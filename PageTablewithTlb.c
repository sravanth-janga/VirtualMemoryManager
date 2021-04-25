#include "types.h"

// TLB uses to LRU mechanism to choose entries to be replaced!
// source code for specification 2:

/*
    Control Flow :

    1.Check if vpn corresponding to the virtual address is present in the TLB.
    2. If present:
            i. check if the entry is valid and process has rights to access it.
                I. if satisfied ( get the physical address)
                II. if not (raise exception and terminatet the process.)
    3. If not present

*/
// pte_t PageTable[64];






pte_t PageTable[64];


char* get_binary(void* n,int l){
    int num = *(int*)n;
   
    char* s = (char*) malloc(sizeof(char)*l+1);
    for(int i=l-1;i>=0;i--){
        if (num&(1<<i)){
            s[i] = '1';
        }else{
            s[i]='0';
        }
    }
    s[l]=0;
    return s;
}

vpn_t get_vpn(vaddr_t virtualAddress){
    if(virtualAddress >=pow(2,16)){
      
            puts("*********************************");
            printf("VirtualAddr: %s  \n",(char*)get_binary(&virtualAddress,16));
        
            puts("");
            puts("*********************************");
        perror("SEGMENTATION_FAULT!");
        puts("Terminating the process");
        _exit(0);
    }
    return ((virtualAddress)& VPN_MASK)>>SHIFT;
};

pte_t get_pte(vpn_t vpn){
    return PageTable[vpn];
}
void free_tlb(tlb_entry_t **head,tlb_entry_t**tail){
    puts("Freeing TLB...");
    sleep(1);
    if(*head==NULL){
        return;
    }
    tlb_entry_t *cur = *head;
    tlb_entry_t *prev = NULL;
    cur = cur->next;
    while(cur!=*head){
        prev = cur;
        cur = cur->next;
        free(prev);
    }
    free(*head);
    puts("Done!");

}

void AssignPFN(){
     puts("Assigning pfns ...");
    srand(time(0));
    for(int i= 0;i<64;i++){
        PageTable[i].PFN = rand() % (int)(pow(2,14));
        PageTable[i].KernelMode = 0;
        // present and protection bits.
        PageTable[i].Present = 1;
        PageTable[i].Protection_bits.read = 1;
        PageTable[i].Protection_bits.write = 1;
        PageTable[i].Protection_bits.execute = 1;
        PageTable[i].Valid = 1;
    };
 

    puts("Done!");
};

void getPhysicalAddress(vaddr_t va, pte_t (*Entry)){
    if((*Entry).Valid==0){
        
            puts("*********************************");
            printf("VirtualAddr: %s | Valid: %d \n",(char*)get_binary(&va,16)
            ,(*Entry).Valid);
            
            puts("");
            puts("*********************************");
        perror("SEGMENTATION_FAULT");
        puts("Terminating the process");
        _exit(0);
      
    }else if (((*Entry).Protection_bits.read==0) & ((*Entry).Protection_bits.write==0))
    {
             
            puts("*********************************");
            printf("VirtualAddr: %s | R: %d | W: %d | X: %d \n",
            (char*)get_binary(&va,16),(*Entry).Protection_bits.read,(*Entry).Protection_bits.write,(*Entry).Protection_bits.execute);
          
            puts("");
            puts("*********************************");
        perror("PROTECTION ERROR!");
        puts("Terminating the process");
        _exit(0);
        
    }else{
            paddr_t offset = va & OFFSET_MASK;
            paddr_t PhysAddr= ((*Entry).PFN << SHIFT)| offset;
            vpn_t VPN = get_vpn(va);

            puts("*********************************");
            printf("VirtualAddr: %s | PhysicalAddr: %s \n",(char*)get_binary(&va,16),
            (char*)get_binary(&PhysAddr,24));
            printf("VPN: %s | Offset: %s \n",get_binary(&VPN,6),get_binary(&offset,10));

            puts("");
            puts("*********************************");

    }
    
}



void TLB_DELETE(tlb_entry_t **head,tlb_entry_t**tail,tlb_size *count){
    tlb_entry_t* cur = (tlb_entry_t*) malloc(sizeof(tlb_entry_t));
    cur = *head;
    tlb_entry_t* prev = (tlb_entry_t*) malloc(sizeof(tlb_entry_t));
    prev = NULL;
    // puts("del started");
    while ((cur)->use==1)
    {   
        
        (cur)->use =0; // change use bit to 0;
        prev = cur;
        cur = (cur)->next;
    };
   
    // printf("%d %d %d\n",prev->VPN,cur->VPN,(cur)->next->VPN);
    if(prev!=NULL){
        *tail = prev;
        // puts("no null");
    }   
    // puts("yes null");
    *head = (cur)->next;
    (*tail)->next = *head;
    

    
    // puts("deletion done!");
    *count--;
}
// head is used as a reference to the variable which points to particular entry.
void TLB_INSERT(tlb_entry_t **head,tlb_entry_t **tail,vpn_t VPN,pfn_t PFN,tlb_size limit,tlb_size* count){
    tlb_entry_t* cur = (tlb_entry_t*) malloc(sizeof(tlb_entry_t));
    cur->VPN = VPN;
    cur->PFN = PFN;
    cur->protect_bit = 1;
    cur->use = 1;
    if (*count>limit){
        TLB_DELETE(head,tail,count);
    };
    
    if (*tail==NULL){
       
        *head = cur;
        *tail = cur;
        (*head)->next = *tail;
        (*tail)->next = *head;

    }else{
        (*tail)->next = cur;
        cur->next = *head;
        *tail = cur;
    }
    *count++;
    // puts("inserted");
    
}
int TLB_LOOKUP(tlb_entry_t **head,tlb_entry_t**tail,vpn_t VPN,tlb_size limit,tlb_size* count){
    tlb_entry_t* cur = (tlb_entry_t*) malloc(sizeof(tlb_entry_t));
    
   
    if ((*head)==NULL){
        puts("*******************");
        puts("<< TLB MISS! >>");
        
        
        pte_t pte = get_pte(VPN);
        pfn_t PFN = pte.PFN;
        
    
        
        TLB_INSERT(head,tail,VPN,PFN,limit,count);
        // puts("INSERT");
        return NULL;
    };
    
    
    cur = *head;
   
    if (cur->VPN==VPN){
        
        puts("<< TLB Hit! >");

        puts("*******************");
        cur->use = 1; 
        return (int) cur->PFN;
    };
      cur = cur->next;
       while ((cur)!=*head)
      {   
        //   puts("looping");
        if ((cur)->VPN==VPN)
        {
   
        puts("<< TLB Hit! >>");
       
        (cur)->use = 1;
        // printf("> %d %d\n",(cur)->VPN,(cur)->use);
      

        return (int) cur->PFN;
        }   
       cur = cur->next;
    };
    
    puts("<< TLB MISS ! >>");

        pte_t pte = get_pte(VPN);
        pfn_t PFN = pte.PFN;
        
        
        TLB_INSERT(head,tail,VPN,PFN,limit,count);
        return -1;

}

void FETCH_PHYS_ADDR(tlb_entry_t **head,tlb_entry_t**tail,vaddr_t va,tlb_size limit,tlb_size* count,int verbose){
    clock_t tm;
    vpn_t VPN = get_vpn(va);
    // puts("VPN done");
    int rc;
    tm = clock();
    rc = TLB_LOOKUP(head,tail,VPN,limit,count);
    // puts("lookup done");
    if (rc==-1){
        FETCH_PHYS_ADDR(head,tail,va,limit,count,verbose);
    }else{
        int pfn = rc;
       
         paddr_t offset = va & OFFSET_MASK;
        
        paddr_t PhysAddr= ((pfn) << SHIFT)| offset;

                if (verbose>0){

               puts("*******************");
        printf("Total time taken to acces TLB :\
                 %fns \n",(double)((tm*1e6)/CLOCKS_PER_SEC));  
                };
        
                    puts("*********************************");
            printf("VirtualAddr: %s | PhysicalAddr: %s \n",get_binary(&va,16),get_binary(&PhysAddr,24));
            printf("VPN: %s | Offset: %s \n",get_binary(&VPN,6),get_binary(&offset,10));
            puts("");
            puts("*********************************");
    }
}

/*

*/

/*
Driver code for specification1  

*/


int main(int argc, char* argv[]){
    
    pte_t pte;
    vpn_t vpn;
    int n_samples  = 30;
    clock_t tm, no_tlb,with_tlb;
    tlb_entry_t* head=NULL;
    tlb_entry_t* tail=NULL;
    tlb_size limit,count=0;
    int TLB_SIZES[4]={0,8,16,32};
    int SIZES[5]={16, 32, 64, 128,256};
    int n_addresses[4] = {25, 50, 75, 100};
    clock_t tot_time,counter;
    AssignPFN();
    vaddr_t virtualAddress;

    if (argc ==2){
        n_samples = atoi(argv[1]);
    }
    if (n_samples==0){
        n_samples = 40;
    }
    for (int siz=0;siz<5;siz++){
    limit =SIZES[siz];
    int i =0;
    free_tlb(&head,&tail);
    head = NULL;
    tail = NULL;
    printf("Comparing with TLB of Size : %d \n",limit);
    while (i<n_samples)
    {
        sleep(1);
        puts(" ");
        printf("Access : %d \n",i+1);
        no_tlb= clock();
        virtualAddress = rand()% (int)(5000);
        
        vpn = get_vpn(virtualAddress);
        pte = get_pte(vpn);
        getPhysicalAddress(virtualAddress,&pte);
        no_tlb = clock()-no_tlb;
        printf("Total time taken:\
        %fmic.sec \n",(double)((no_tlb*1e6)/CLOCKS_PER_SEC)); 
        puts("********************");



        with_tlb= clock();
        
        FETCH_PHYS_ADDR(&head,&tail,virtualAddress,limit,&count,-1);
        with_tlb = clock()-with_tlb;

        printf("Total time taken:\
        %f mic.sec \n",(double)((with_tlb*1e6)/CLOCKS_PER_SEC)); 
        puts("********************************************************");
        i+=1;

        printf("Difference :\
        %f mic.sec \n",((double)((with_tlb*1e6)/CLOCKS_PER_SEC)-(double)((no_tlb*1e6)/CLOCKS_PER_SEC))); 
        puts("****************************************************************");
        i+=1;
    }
    
    };
    sleep(2);
    puts("Starting Experiment!.....");


    for (int n_add = 0;n_add<4;n_add++){
    for (int i=0;i<4;i++){
        limit = TLB_SIZES[i];
        free_tlb(&head,&tail);
         head = NULL;
        tail = NULL;
        if (limit==0){
            // no tlb
        tot_time = 0;
        for (int j=0;j<10;j++){
            sleep(1);
            printf("Trial : %d \n",j+1);
            for(int k=0;k<n_addresses[n_add];k++){
        
        puts(" ");
        printf("S.No : %d \n",k+1);
        counter = clock();
        virtualAddress = rand()% (int)(pow(2,16));
        
        vpn = get_vpn(virtualAddress);
        pte = get_pte(vpn);
        getPhysicalAddress(virtualAddress,&pte);
        counter = clock()-counter;
        tot_time = tot_time+counter;
            };
        };
        puts("------------------------");
        printf("Average Time Without TLB:\
                 %fmic.sec \n",(double)(tot_time*1e5/CLOCKS_PER_SEC)); 
        puts("");
        puts("------------------------");
        }else{
            
            count = 0;
        tot_time = 0;
        for (int j=0;j<10;j++){
            sleep(1);
            printf("Trial : %d \n",j+1);
            for(int k=0;k<n_addresses[n_add];k++){
        
        puts(" ");
        printf("S.No : %d \n",k+1);
        counter = clock();
        virtualAddress = rand()% (int)(pow(2,16));
        FETCH_PHYS_ADDR(&head,&tail,virtualAddress,limit,&count,-1);
        counter = clock()-counter;
        tot_time = tot_time+counter;
            };
        };

        puts("------------------------");
        printf("Average Time With TLB:\
                 %fmic.sec \n",(double)(tot_time*1e5/CLOCKS_PER_SEC)); 
        puts("------------------------");
        };
        puts("Initializing next trial ....");
        sleep(3);
    }};
    

    return 0;
}