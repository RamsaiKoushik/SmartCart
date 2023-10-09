#include"stdio.h"
#include"unistd.h"
#include"fcntl.h"

typedef struct person {
    char name[50];
    char emailaddress[50];
    char password[50];
} person;

typedef struct cartItem{
    int productId;
    int quantity;
} cartItem;

typedef struct customer{
    int customerId;
    person details;
    cartItem cart[50];
    int assigned;
} customer;

typedef struct product{
    int productId;
    char productName[50];
    int productPrice;
    int productQuantity;
} product;

int main()
{
    int fd;
    //initialzing some containers for storing customers
    fd = open("customers.txt",O_CREAT | O_RDWR | O_TRUNC,0744);
    customer cust;
    for(int i=0;i<50;i++)
    {
        cust.cart[i].productId=0;
        cust.cart[i].quantity=-1;
    }
    cust.assigned=0;////indicating block not occupied
    for(int i=0;i<1500;i++)
    {
        cust.customerId=i+1;
        write(fd,&cust,sizeof(customer));
    }

    //initialzing some containers for storing products
    fd = open("products.txt",O_CREAT | O_RDWR | O_TRUNC ,0744);
    product p;
    p.productQuantity=-1;//indicating block not occupied
    for(int i=0;i<2000;i++)
    {
        p.productId=i+1;
        write(fd,&p,sizeof(product));
    }
}