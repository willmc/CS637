struct proc_node_elmt {
    int pid;
    char* mem;
    struct proc_node_elmt * next;
};

typedef struct proc_node_elmt proc_node;

proc_node* head_proc_node;
proc_node* curr_proc_node;
int numProcs;
int hasOneProc;

void addProc(int i);
void printProcList();
void clearProcList();
