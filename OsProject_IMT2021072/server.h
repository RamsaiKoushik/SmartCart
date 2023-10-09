#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"netinet/in.h"
#include"unistd.h"
#include"stdbool.h"
#include"string.h"
#include"fcntl.h"
#include"arpa/inet.h"

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

typedef struct result{
    int res;
    char reason[50];
} result;

int UserMenu(int socketDescriptor);
int Menu(int socketDescriptor,int currUserId);
void updateCarts(int productId,int quantity);
void UpdateProduct(int socketDescriptor,int currUserId);
void RemoveProduct(int socketDescriptor,int currUserId);
void AddProduct(int socketDescriptor,int currUserId);
void UpdateProductInCart(int socketDescriptor,int currUserId);
void RemoveProductFromCart(int socketDescriptor,int currUserId);
void receiptNumber(int socketDescriptor);
void PurchaseCart(int socketDescriptor,int currUserId);
product BuyProductById(int id,int quantity);
int ProductCheck(cartItem userItem);
void AddProductToCart(int socketDescriptor,int currUserId);
void DisplayCartItems(int socketDescriptor,int currUserId);
int Register(int socketDescriptor,customer user);
int ValidateUser(int socketDescriptor,customer user,int type);
void ListAllProducts(int socketDescriptor);
product ProductById(int id);



