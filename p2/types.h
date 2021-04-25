#ifndef _TYPES_H_
#define _TYPES_H_
#include<math.h>
#include <netinet/in.h> /* For uintXX_t types */
#include<stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include<unistd.h>
#include<sys/time.h>
// BIT masks
#define VPN_MASK 1047552
#define  SHIFT 10
#define OFFSET_MASK 1023
#define PD_MASK 1572864
#define PD_SHIFT 18
/*******************************************************************************
 * Virtual addresses are 16 bits.  
 */
typedef uint16_t tlb_size;


typedef uint16_t vaddr_t;
/*******************************************************************************
 * Physical addresses are 24 bits.
 */
typedef uint32_t paddr_t;

/*******************************************************************************
 * Virtual page numbers can be up to 6 bits. For pedantic reasons.
 */
typedef uint8_t vpn_t;

// pte_addr


/*******************************************************************************
 * Physical frame numbers can be up to 14 bits.
 */
typedef uint16_t pfn_t;

/* flag bit True : 1 or False : 0*/
typedef u_int8_t flag_t;
//  protect bits;

typedef struct{
    flag_t read;
    flag_t write;
    flag_t execute;
}protect_t;


// page table entry:
typedef struct {
    pfn_t PFN;
    flag_t Valid;
    flag_t Present;
    protect_t Protection_bits;
    flag_t KernelMode;  // true : kernel mode | false : usermode
} pte_t;


// As we are not doing any context 
//switch we don't need extra bits like valid , presen etc..

typedef struct tlb_node{   
    vpn_t VPN;
    pfn_t PFN;
    uint8_t use;
    uint8_t protect_bit;
    struct tlb_node* next;
} tlb_entry_t;

typedef struct pde{
    int valid;
    pte_t* PFN;
}pde_t;
// useful functions!

void AssignPFN();
pte_t get_pte(vpn_t vpn);
void getPhysicalAddress(vaddr_t va, pte_t (*Entry));
pte_t get_pte(vpn_t vpn);
vpn_t get_vpn(vaddr_t virtualAddress);
void traverse(tlb_entry_t **head,vpn_t VPN);
void TLB_DELETE(tlb_entry_t **head,tlb_entry_t**tail,tlb_size* count);
void TLB_INSERT(tlb_entry_t **head,tlb_entry_t **tail,vpn_t VPN,pfn_t PFN,tlb_size limit,tlb_size* count);
int TLB_LOOKUP(tlb_entry_t **head,tlb_entry_t**tail,vpn_t VPN,tlb_size limit,tlb_size* count);
void FETCH_PHYS_ADDR(tlb_entry_t **head,tlb_entry_t**tail,vaddr_t va,tlb_size limit,tlb_size* countv,int verbose);

#endif