#include"stdio.h"
#include"stdlib.h"
#include"sys/types.h"
#include"sys/socket.h"
#include"netinet/in.h"
#include"arpa/inet.h"
#include"unistd.h"
#include"fcntl.h"
#include"string.h"
#include"time.h"

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
int Register(int socketDescriptor);
int ValidateUser(int socketDescriptor,int type);
int AdminMenu(int socketDescriptor);
int CustomerMenu(int socketDescriptor);
void ListAllProducts(int socketDescriptor);
void DisplayCartItems(int socketDescriptor);
void StockLevels(int socketDescriptor);
// product ProductById(int socketDescriptor,int id);
void AddProductToCart(int socketDescriptor);
void UpdateProductInCart(int socketDescriptor);
void RemoveProductFromCart(int socketDescriptor);
void PurchaseCart(int socketDescriptor);
void AddProduct(int socketDescriptor);
void RemoveProduct(int socketDescriptor);
void UpdateProduct(int socketDescriptor);


