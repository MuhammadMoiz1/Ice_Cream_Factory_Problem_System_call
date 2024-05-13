#include<linux/semaphore.h>
#include<linux/kthread.h>
#include<linux/delay.h>
#include<linux/unistd.h>
#include<linux/module.h>
#include<linux/init.h>


#define MAX_CUSTOMERS 30


struct semaphore tokenc;
struct semaphore flavorc;
struct semaphore topc;
struct semaphore cashc;
struct semaphore ice_mach;
struct semaphore f1,f2,f3;
static int tsale=0;
int ticket=30;


int top=20;
int rf1=0,rf2=0,rf3=0;
int tf1=12,tf2=15,tf3=12;
void* p1(void *prod) {

while(tf1>0){

down(&ice_mach);

down(&f1);

                   rf1++;

                   tf1--;

 //rf1+=3;

 //tf1-=3;

 printk("Producer 1 ,produced a scoop of Chocolate icecream , Total scoops remaining: %d\n",rf1);

up(&f1);

up(&ice_mach);

msleep(10);

}

return NULL;

}

void* p2(void *prod) {

while(tf2>0){

down(&ice_mach);

down(&f2);

                  rf2++;

                   tf2--;

 //rf2+=3;

 //tf2-=3;

 printk("Producer 2 ,produced a scoop of Mango icecream , Total scoops remaining: %d\n",rf2);

up(&f2);



up(&ice_mach);

msleep(10);

}

return NULL;

}

void* p3(void *prod) {

while(tf3>0){

down(&ice_mach);

down(&f3);

                   rf3++;

                   tf3--;

 //rf3+=3;

 //tf3-=3;

 printk("Producer 3 ,produced a scoop of Vanilla icecream , Total scoops remaining: %d\n",rf3);

up(&f3);



up(&ice_mach);

msleep(10);

}

return NULL;

}

void* icecream_shop (void *customer) {

   

    int c= *(int*) customer;

    int bill=0;

    int rc=0;

   

   //tocken counter start

    down(&tokenc);

   

    printk("Customer %d, arrived.\n", c);

    if(ticket<=0){

printk("\nCustomer %d, Leaving Shop. \n[REASON]: Tickets Finished\n",c);

return NULL;

}

ticket--;

printk("Customer %d, got ticket.\n", c);

    msleep(2);



    up(&tokenc);

   

    //flavour counter start

down(&flavorc);

printk("\nCustomer %d,entered flavour counter \n",c);

if(tf1<=0&&tf2<=0&&tf3<=0&&rf1<=0&&rf2<=0&&rf3<=0){

printk("\nCustomer %d, Leaving Shop. [REASON]: Flavours Finished\n",c);
        up(&flavorc);
return NULL;

}

else{



         

        if(c%4==3||c%4==0) {

while(rf1<=0&&tf1>0)
printk("*");

if(tf1>0||rf1>0)

{  

   

   down(&f1);

rf1--;



rc++;

bill += 80;

printk("Customer %d, Got chocolate Flavour scoop of icecream. Remaining scoops: %d\n",c,rf1);

up(&f1);

}



}

if(c%4==3||c%4==1) {

while(rf2<=0&&tf2>0)
printk("*");

if(tf2> 0||rf2>0)

{  

 

   down(&f2);

           

rf2--;

   

rc++;

bill+=80;

printk("Customer %d, Got mango Flavour scoop of icecream. Remaining scoops: %d\n",c,rf2);

up(&f2);

}

}

if(c%4==3||c%4==2) {

while(rf3<=0&&tf3>0);

if(tf3> 0||rf3>0)

{

while(rf3<=0&&tf3>0)
printk("*");

   down(&f3);

     

rf3--;

rc++;

bill += 90;

printk("Customer %d, Got vanilla Flavour scoop of icecream. Remaining scoops: %d\n",c,rf3);

up(&f3);

}

}

if(rc== 0)

{

   printk("\nCustomer %d, Leaving Shop. [REASON]: Flavours Finished\n",c);
            up(&flavorc);
return NULL;

}

}



printk("\nCustomer %d, Got Icecream. Leaving Flavor Counter\n",c);



msleep(2);



    up(&flavorc);

   

   

    //topping counter start

    if(c%2==0){

    down(&topc);

    if(top> 0)

{

top--;

bill += 30;

printk("\nCustomer %d, Got topping. Leaving topping Counter\n",c);

}

else{



printk("\nToppings Finished, Customer %d, Leaving topping Counter\n",c);



}



    msleep(2);

    up(&topc);

    }

    //cash counter start

    down(&cashc);

   

printk("\nCustomer %d, Billed: %d RS.\n",c,bill);

tsale=tsale+bill;

printk("\n\nCustomer %d: Leaving Ice-Cream Shop.\n\n",c);

msleep(2);

    up(&cashc);

   

    return NULL;

}

   

asmlinkage long sys_IcecreamFact(void){



    printk(KERN_INFO "\n\n\n***************** WELCOME TO THE ICE-CREAM SHOP *************************\n\n\n");


   int customer[MAX_CUSTOMERS];
   int prod[3]={1,2,3};
    struct task_struct *customer_id[MAX_CUSTOMERS];
    struct task_struct *prod_id[3];


    sema_init(&tokenc,1);
    sema_init(&flavorc, 1);
    sema_init(&topc, 1);
    sema_init(&cashc,1);
    sema_init(&f1,1);
    sema_init(&f2,1);
    sema_init(&f3,1);
    sema_init(&ice_mach,1);

       

        //creating producers thread

        prod_id[0] = kthread_create((void*)p1, (void *)&prod[0],"icecream_shop");
        if(prod_id[0]){
            wake_up_process(prod_id[0]);
        }
        else{
        kthread_stop(prod_id[0]);

}

       

       

        prod_id[1] = kthread_create((void*)p2, (void *)&prod[1],"icecream_shop");

        if(prod_id[1]){
            wake_up_process(prod_id[1]);
        }
        else{
        kthread_stop(prod_id[1]);
}

        prod_id[2] = kthread_create((void*)p3, (void *)&prod[2],"icecream_shop");
        if(prod_id[2]){
            wake_up_process(prod_id[2]);
        }
        else{
        kthread_stop(prod_id[2]);      
}

       


    int i;

    for (i = 0; i < MAX_CUSTOMERS; i++) {

    customer[i] = i+1;
        customer_id[i] = kthread_create((void*)icecream_shop, (void *)&customer[i],"icecream_shop");
        if(customer_id[i]){
            wake_up_process(customer_id[i]);
        }
        else{
        // printk("Failed to create consumer thread\n");
        kthread_stop(customer_id[i]);
}

    }

   msleep(10);

    return 0;

}
