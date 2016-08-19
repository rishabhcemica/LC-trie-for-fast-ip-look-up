#include<stdio.h>
/* extract n bits from str starting at position p */
//#define EXTRACT(p, n, str) ((str)<<(p)>>(32-(n)))

#define MAXIP 10
#define MAXSIZE 100                                        
#define REMOVE(p, str)   ((str)<<(p)>>(p))       /* remove the first p bits from string */
#define SETBRANCH(branch)   ((branch)<<27)       /*set 5 bits to the branch*/
#define GETBRANCH(node)     ((node)>>27)         /*get branch value*/
#define SETSKIP(skip)       ((skip)<<22)                                
#define GETSKIP(node)       ((node)>>22 & 037)
#define SETADR(adr)         (adr)
#define GETADR(node)        ((node) & 017777777)

static double FILLFACT = 0.50;                      
unsigned int base[MAXIP];
int trie[MAXSIZE];
struct routing_table{
	unsigned int *trie;
	int nexthop;
};
struct routing_table t[MAXIP];
unsigned EXTRACT(unsigned p, unsigned n,unsigned str){ return ((str)<<(p)>>(32-(n)));}

/*computing the branch value for a node */
void computeBranch(unsigned int base[], int prefix, int first, int n,
                   int *branch, int *newprefix)
{
   int low, high;
   int i, pat, b;
   int patfound;
   int count;

   /* Compute the new prefix */
   high = REMOVE(prefix, base[first])<0 ? (-1)*(REMOVE(prefix, base[first])) : REMOVE(prefix, base[first]) ;
   low = REMOVE(prefix, base[first+n-1])<0 ? REMOVE(prefix, base[first+n-1])*(-1) : REMOVE(prefix, base[first+n-1]) ;
   i = prefix;
   while ((EXTRACT(i, 1, low)<0 ? (-1)*(EXTRACT(i, 1, low)) :EXTRACT(i, 1, low)) == (EXTRACT(i, 1, high)<0 ? (-1)*(EXTRACT(i, 1, high)):EXTRACT(i, 1, high)))
      i++;
   *newprefix = i;

   /* Always use branching factor 2 for two elements */
   if (n == 2) {
      *branch = 1;
      return;
   }

   /* Compute the number of bits that can be used for branching.
      We have at least two branches. Therefore we start the search
      at 2^b = 4 branches. */
   b = 1;
   do {
      b++;
      if (n < FILLFACT*(1<<b) ||
          *newprefix + b >32)
         break;
      i = first;
      pat = 0;
      count = 0;
      while (pat < 1<<b) {
         patfound = 0;
         while (i < first + n &&   
                pat == (EXTRACT(*newprefix, b, base[i])<0 ? (-1)*(EXTRACT(*newprefix, b, base[i])) : EXTRACT(*newprefix, b, base[i]))){
            i++;
            patfound = 1;
         }
         if (patfound)
            count++;
         pat++;
      }
   } while (count >= FILLFACT*(1<<b));
   *branch = b-1;
}
void build(unsigned int base[], int prefix, int first, int n,
           int pos, int *nextfree, int trie[])
{
   int branch, newprefix;
   int k, p, adr;
   int bitpat;

   if (n == 1)
      trie[pos] = first; /* branch and skip are 0 */
   else if(n==0)
       return;
   else {
      computeBranch(base, prefix, first, n, &branch, &newprefix);
      adr = *nextfree;
      trie[pos] = SETBRANCH(branch) |
                  SETSKIP(newprefix-prefix) |
                  SETADR(adr);
      *nextfree += 1<<branch;
      p = first;
      /* Build the subtrees */
      for (bitpat = 0; bitpat < 1<<branch; bitpat++) {
         k = 0;         
         while ( p+k < first+n && (EXTRACT(newprefix, branch, base[p+k])<0 ? (-1)*(EXTRACT(newprefix, branch, base[p+k])) :EXTRACT(newprefix, branch, base[p+k]))== bitpat)
            k++;

        build(base,newprefix+branch, p, k,adr + bitpat, nextfree, trie);
         p += k;
      }
   }
}
int find(int s)
{
   int node;
   int pos, branch, adr;

   /* Traverse the trie */
   node =trie[0];
   pos = GETSKIP(node);
   branch = GETBRANCH(node);
   adr = GETADR(node);
   while (branch != 0) {
      node =trie[adr + (EXTRACT(pos, branch, s)<0 ? (-1)*(EXTRACT(pos, branch, s)):EXTRACT(pos, branch, s)) ];
      pos += branch + GETSKIP(node);
      branch = GETBRANCH(node);
      adr = GETADR(node);
   }
      return t[adr].nexthop;
   }
int main()
{
int nip,i,ipaddr;
int nextfree=1;
int hop;
int ip1;
   printf("enter the number of ipaddress");
     
     scanf("%d",&nip);
     for(i=0;i<nip;i++)
     {
        printf("enter the ip address in decimal and nexthop in decimal");
        scanf("%d %d",&ipaddr,&hop);
       base[i]=ipaddr;
       t[i].trie=&base[i];
       t[i].nexthop=hop;
        
     }
  build(base,0,0,nip,0,&nextfree,trie);
   printf("enter the ip for which nexthop you want to find");
   scanf("%d",&ip1);
   printf("%d",find(ip1));
return 0;
}




