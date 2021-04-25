#include "types.h"
/* 
Source code for specification 1:
    Assumptions:

    virtual address space  = 16-bits (0 to 2^16 -1)
    RAM =  16MB  ( 2^24/2^10 = 2^14 physicalFrames)
    page size =  1KB  (10 bits offset);
    => no.of PTEs = (2^16/2^10 )= 2^6;
    => VPN_MASK = (2^15 + 2^14 +---+ 2^10) ( 6-bits);

*/
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


/*
Driver code for specification1  

*/


int main(int argc, char* argv[]){
    
    vpn_t vpn;
    pte_t pte;
    int n_samples  = 30;
    clock_t tm;
    AssignPFN();
    vaddr_t virtualAddress;
    if (argc ==2){
        n_samples = atoi(argv[1]);
    }
    if (n_samples==0){
        n_samples = 50;
    }
    int i =0;
    while (i<n_samples)
    {
        sleep(1);
        puts(" ");
        printf("Access : %d \n",i+1);
        tm = clock();
        virtualAddress = rand()%(50000);
        
        vpn = get_vpn(virtualAddress);
        pte = get_pte(vpn);
        getPhysicalAddress(virtualAddress,&pte);
        tm = clock()-tm;
        printf("Total time taken:\
        %f mic.sec \n",(double)((tm*1e6)/CLOCKS_PER_SEC)); 
        puts("********************");
        i+=1;
    }
    
    return 0;
}