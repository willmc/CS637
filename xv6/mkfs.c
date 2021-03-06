#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include "types.h"
#include "fs.h"

#define BSIZEL 512L

int nblocks = 1997;
int ninodes = 300;
int size = 2048;

int fsfd;
struct superblock sb;
char *zeroes;
uint freeblock;
uint usedblocks;
uint bitblocks;
uint freeinode = 1;

void balloc(int);
void wsect(uint, void*);
void winode(uint, struct dinode*);
void rinode(uint inum, struct dinode *ip);
void rsect(uint sec, void *buf);
uint ialloc(ushort type);
void iappend(uint inum, void *p, int n);

// convert to intel byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar*) &y;
  a[0] = x;
  a[1] = x >> 8;
  return y;
}

uint
xint(uint x)
{
  uint y;
  uchar *a = (uchar*) &y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  return y;
}

int
main(int argc, char *argv[])
{
  int i, cc, fd;
  uint rootino, inum, off;
  struct dirent de;
  char buf[BSIZE];
  struct dinode din;

  if(argc < 2){
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  assert((BSIZE % sizeof(struct dinode)) == 0);
  assert((BSIZE % sizeof(struct dirent)) == 0);

  fsfd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
  if(fsfd < 0){
    perror(argv[1]);
    exit(1);
  }

  sb.size = xint(size);
  sb.nblocks = xint(nblocks); // so whole disk is size sectors
  sb.ninodes = xint(ninodes);

  //bitblocks = size/(BSIZE*8) + 1;
  //usedblocks = ninodes / IPB + 3 + bitblocks;
  bitblocks = 3;
  usedblocks = 39 + bitblocks + 9;
  freeblock = usedblocks;

  fprintf(stderr,"used %d (bit %d ninode %lu) free %u total %d\n", usedblocks,
         bitblocks, 39, freeblock, nblocks+usedblocks);

  assert(nblocks + usedblocks == size);



    zeroes = (char*)calloc(BSIZE, 1);
    for(i = 0; i < nblocks + usedblocks; i++)
    {
        wsect(i, zeroes);
    }
    fprintf(stderr,"TOKEN: Done zeroing file. Writing super block on each cylinder.\n");

  wsect(1, &sb);
  wsect(683, &sb);
  wsect(1364, &sb);

  fprintf(stderr,"Adding . and .. to each cylinder's root.\n");
  uchar buf2[512];
  bzero(buf2, 512);
  wsect(2, buf2);
  wsect(3, buf2);
  wsect(684, buf2);
  wsect(685, buf2);
  wsect(1366, buf2);
  wsect(1367, buf2);


  usedblocks = 0;
  rootino = ialloc(T_DIR);
  assert(rootino == 1);

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  fprintf(stderr,"Appending . to root inode\n");
  iappend(rootino, &de, sizeof(de));

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  fprintf(stderr,"Appending .. to root inode\n");
  iappend(rootino, &de, sizeof(de));


  fprintf(stderr,"Looping over args\n");
  for(i = 2; i < argc; i++){
    assert(index(argv[i], '/') == 0);

    if((fd = open(argv[i], 0)) < 0){
      perror(argv[i]);
      exit(1);
    }
    
    // Skip leading _ in name when writing to file system.
    // The binaries are named _rm, _cat, etc. to keep the
    // build operating system from trying to execute them
    // in place of system binaries like rm and cat.
    if(argv[i][0] == '_')
      ++argv[i];

    inum = ialloc(T_FILE);

    bzero(&de, sizeof(de));
    de.inum = xshort(inum);
    strncpy(de.name, argv[i], DIRSIZ);
    fprintf(stderr,"Appending command %s to root inode.\n", argv[i]);
    iappend(rootino, &de, sizeof(de));

    while((cc = read(fd, buf, sizeof(buf))) > 0)
      iappend(inum, buf, cc);

    close(fd);
    fprintf(stderr,"Done with command %s\n", argv[i]);
  }

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.size);
  off = ((off/BSIZE) + 1) * BSIZE;
  din.size = xint(off);
  fprintf(stderr,"Final winode on root inode.\n");
  winode(rootino, &din);

  fprintf(stderr,"Final balloc, given usedblocks\n");
  balloc(usedblocks);

  exit(0);
}

void
wsect(uint sec, void *buf)
{
  if(lseek(fsfd, sec * BSIZEL, 0) != sec * BSIZEL){
    perror("lseek");
    exit(1);
  }
  if(write(fsfd, buf, BSIZE) != BSIZE){
    perror("write");
    exit(1);
  }
}

uint
i2b(uint inum)
{
  return (inum / 100) * 682 + 4 + (inum % 100) / IPB;
}

void
winode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = i2b(inum);
  rsect(bn, buf);
  dip = ((struct dinode*) buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

void
rinode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = i2b(inum);
  rsect(bn, buf);
  dip = ((struct dinode*) buf) + (inum % IPB);
  *ip = *dip;
}

void
rsect(uint sec, void *buf)
{
  if(lseek(fsfd, sec * BSIZEL, 0) != sec * 512L){
    perror("lseek");
    exit(1);
  }
  if(read(fsfd, buf, BSIZE) != 512){
    perror("read");
    exit(1);
  }
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  struct dinode din;

  bzero(&din, sizeof(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);

  uchar buf[512];
  if (inum == 1)
  {
    bzero(buf, 512);
    buf[inum/8] = buf[inum/8] | (0x1 << (inum%8));
  }
  else 
  {
    rsect(2, buf);
    buf[inum/8] = buf[inum/8] | (0x1 << (inum%8));
  }
  wsect(2, buf);
  return inum;
}

void
balloc(int used)
{
  uchar buf[BSIZE];
  int i;

  fprintf(stderr, "balloc: first %d blocks have been allocated\n", used);
  //assert(used < BSIZE);
  bzero(buf, BSIZE);
  for(i = 0; i < used; i++) {
    buf[i/8] = buf[i/8] | (0x1 << (i%8));
  }
  fprintf(stderr, "balloc: write bitmap block at sector %lu\n", 3);
  wsect(3, buf);

  /*
  bzero(buf,512);
  for(i = 0; i < freeinode; i++)
  {
    buf[i/8] = buf[i/8] | (0x1 << (i%8));
  }
  wsect(2,buf);
  */
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void
iappend(uint inum, void *xp, int n)
{
  char *p = (char*) xp;
  uint fbn, off, n1;
  struct dinode din;
  char buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);

  off = xint(din.size);
  while(n > 0){
    fbn = off / BSIZE;
    assert(fbn < MAXFILE);
    if(fbn < NDIRECT) {
      if(xint(din.addrs[fbn]) == 0) {
        din.addrs[fbn] = xint(freeblock++);
        usedblocks++;
      }
      x = xint(din.addrs[fbn]);
    } else {
      if(xint(din.addrs[INDIRECT]) == 0) {
        // fprintf(stderr,"allocate indirect block\n");
        din.addrs[INDIRECT] = xint(freeblock++);
        usedblocks++;
      }
      // fprintf(stderr,"read indirect block\n");
      rsect(xint(din.addrs[INDIRECT]), (char*) indirect);
      if(indirect[fbn - NDIRECT] == 0) {
        indirect[fbn - NDIRECT] = xint(freeblock++);
        usedblocks++;
        wsect(xint(din.addrs[INDIRECT]), (char*) indirect);
      }
      x = xint(indirect[fbn-NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.size = xint(off);
  winode(inum, &din);
}
