#include"client.h"

int UserMenu(int socketDescriptor)
{
    printf("\nUserMenu:\n");
    printf("1.Admin\n");
    printf("2.Customer\n");
    printf("3.Exit\n");
    printf("Please provide your choice\n");

    int choice,e;
    scanf("%d",&choice);

    int signChoice;
    switch(choice){
        case 1:
            if(ValidateUser(socketDescriptor,3))
                return choice;
            else
                UserMenu(socketDescriptor);
            break;
        case 2:
            printf("1.Register\n2.Login\n");
            scanf("%d",&signChoice);
            switch(signChoice){
                case 1:
                    if(Register(socketDescriptor)) 
                        return choice;
                    else
                        UserMenu(socketDescriptor);
                    break;
                case 2:
                    if(ValidateUser(socketDescriptor,2))
                        return choice;
                    else
                        UserMenu(socketDescriptor);
                    break;
                default:
                    UserMenu(socketDescriptor);
            } 
            break;
        case 3:
            e = 4;
            write(socketDescriptor,&e,sizeof(int));
            e=-1;
            return e;
        default:
            UserMenu(socketDescriptor);
    }
}

//Register/login:
int Register(int socketDescriptor)
{
    int option=1;
    write(socketDescriptor,&option,sizeof(option));
    
    person cust_details;
    printf("name: ");
    scanf("%s",cust_details.name);
    printf("emailAddress: ");
    scanf("%s",cust_details.emailaddress);
    printf("password: ");
    scanf("%s",cust_details.password);

    customer c;
    c.details = cust_details;

	write(socketDescriptor,&c,sizeof(customer));	
	
    int valid=0;
	read(socketDescriptor,&valid,sizeof(int));

    if(valid)
        printf("Registration Sucessfull\n");
    else
        printf("Registration Unsucessfull...account already exists\n");

    return valid;
}

int ValidateUser(int socketDescriptor,int type)
{
    customer cust;
    int re,rp;
    printf("emailaddress: ");
    scanf("%s",cust.details.emailaddress);
    printf("password: ");
    scanf("%s",cust.details.password);
   
    int option=type;
    write(socketDescriptor,&option,sizeof(int));
	write(socketDescriptor,&cust,sizeof(customer));	
	
    int valid=10;
	read(socketDescriptor,&valid,sizeof(int));

    if (valid)
        printf("Login Succesful\n");
    else
        printf("Login Invalid\n");
    
    return valid;
}

//admin functions:
int AdminMenu(int socketDescriptor)
{
    printf("\nAdmin Menu:\n");
    printf("1. Add a product\n");
    printf("2. Remove a product\n");
    printf("3. Update a product\n");
    printf("4. List all Products\n");
    printf("5. Logout\n");
    printf("Please provide your choice\n");

    int choice;int logout;
    scanf("%d",&choice);

    switch(choice){
        case 1:
            AddProduct(socketDescriptor);
            break;
        case 2:
            RemoveProduct(socketDescriptor);
            break;
        case 3:
            UpdateProduct(socketDescriptor);
            break;
        case 4:
            ListAllProducts(socketDescriptor);
            break;
        case 5:
            logout=10;
            write(socketDescriptor,&logout,sizeof(int));
            StockLevels(socketDescriptor);
            return 1;
            break;
        default:
            AdminMenu(socketDescriptor);
            break;
    }
    return 0;
}

void StockLevels(int socketDescriptor)
{
    int productCount;
    result count;
    read(socketDescriptor,&count,sizeof(result));
    productCount=count.res;


    product allProducts[productCount];
    read(socketDescriptor,allProducts,sizeof(allProducts));

    char str[200];

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(str,"%d-%02d-%02d_%02d_%02d_%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    strcat(str,".txt");

    int fd = creat(str,O_WRONLY | 0744);
    sprintf(str,"List of products after the latest update by admin:\n");
    write(fd,str,strlen(str));

    for(int i=0;i<productCount;i++){
        sprintf(str,"ProductId:%d , Product Name: %s , Product Price:%d , Product Quantity:%d\n",allProducts[i].productId,allProducts[i].productName,allProducts[i].productPrice,allProducts[i].productQuantity);
        write(fd,str,strlen(str));
    }
    close(fd);
}

void AddProduct(int socketDescriptor)
{
    int choice = 7;
    write(socketDescriptor,&choice,sizeof(int));

    product p;
    printf("Please provide the details of the product,i.e,name,price,quantity with a space in between them\n");
    scanf("%s%d%d",p.productName,&p.productPrice,&p.productQuantity);

    write(socketDescriptor,&p,sizeof(product));
    
    int res;
    read(socketDescriptor,&res,sizeof(int));

    if(!res)
        printf("Product was not added due to space issues or provided invalid data\n");
    if(res>0)
        printf("Product added sucessfully, product is assigned with productId:%d\n",res);
}

void RemoveProduct(int socketDescriptor)
{
    int choice = 8;
    write(socketDescriptor,&choice,sizeof(int));

    product p;
    printf("Please provide the productId of the product you would like to remove\n");
    scanf("%d",&p.productId);

    write(socketDescriptor,&p,sizeof(product));

    int res;
    read(socketDescriptor,&res,sizeof(int));

    if(!res)
        printf("Product does not exist\n");
    if(res)
        printf("Product removed sucessfully\n");
}

void UpdateProduct(int socketDescriptor)
{
    int choice = 9;
    write(socketDescriptor,&choice,sizeof(int));

    product p;
    printf("Please provide the productId along with the price and quantity you would like to update to\n,i.e,id,price,quantity with a space in between them\n");
    scanf("%d %d %d",&p.productId,&p.productPrice,&p.productQuantity);

    write(socketDescriptor,&p,sizeof(product));
    
    int res;
    read(socketDescriptor,&res,sizeof(int));
    if(!res)
        printf("Product does not exist or provided invalid data\n");
    else
        printf("Product updated sucessfully\n");
}

int CustomerMenu(int socketDescriptor)
{
    printf("\nCustomer Menu:\n");
    printf("1. List all the products\n");
    printf("2. Display cart items\n");
    printf("3. Add product to cart\n");
    printf("4. Update a product in cart\n");
    printf("5. Remove a product from cart\n");
    printf("6. Purchase Cart\n");
    printf("7. Logout\n");
    printf("Please provide your choice\n");

    int choice;
    scanf("%d",&choice);

    switch(choice){
        case 1:
            ListAllProducts(socketDescriptor);
            break;
        case 2:
            DisplayCartItems(socketDescriptor);
            break;
        case 3:
            AddProductToCart(socketDescriptor);
            break;
        case 4:
            UpdateProductInCart(socketDescriptor);
            break;
        case 5:
            RemoveProductFromCart(socketDescriptor);
            break;
        case 6:
            PurchaseCart(socketDescriptor);
            break;
        case 7:
            choice=11;
            write(socketDescriptor,&choice,sizeof(int));
            return 1;
            break;
        default:
            CustomerMenu(socketDescriptor);        
    }
    return 0;
} 

void ListAllProducts(int socketDescriptor){
    int choice = 1;
    write(socketDescriptor,&choice,sizeof(int));

    int productCount;
    result count;
    read(socketDescriptor,&count,sizeof(result));
    productCount=count.res;
    product allProducts[productCount];
    read(socketDescriptor,allProducts,sizeof(allProducts));

    printf("\nList of Products:\n");
    for(int i=0;i<productCount;i++){
        printf("ProductId:%d , Product Name: %s , Product Price:%d , Product Quantity:%d\n",allProducts[i].productId,allProducts[i].productName,allProducts[i].productPrice,allProducts[i].productQuantity);
    }
    printf("\n");
}

void DisplayCartItems(int socketDescriptor){
    int choice = 2;
    write(socketDescriptor,&choice,sizeof(int));

    int productCount;
    read(socketDescriptor,&productCount,sizeof(int));

    product allProducts[productCount];
    read(socketDescriptor,allProducts,sizeof(product)*productCount);
    printf("\nList of cart items:\n");
    for(int i=0;i<productCount;i++){
        printf("ProductId:%d , Product Name: %s , Product Price:%d , Product Quantity:%d\n",allProducts[i].productId,allProducts[i].productName,allProducts[i].productPrice,allProducts[i].productQuantity);
    }
}

void AddProductToCart(int socketDescriptor)
{
    int choice = 3;
    write(socketDescriptor,&choice,sizeof(int));

    cartItem item;
    printf("Please provide the productId and quantity to add the product to cart\n");
    scanf("%d%d",&item.productId,&item.quantity);

    write(socketDescriptor,&item,sizeof(cartItem));
    int res;
    read(socketDescriptor,&res,sizeof(int));

    if(!res)
        printf("Product does not exist or sufficient quantity not availble or provided invalid data\n");
    else
        printf("Product added to cart sucessfully\n");
}

void UpdateProductInCart(int socketDescriptor)
{
    int choice = 4;
    write(socketDescriptor,&choice,sizeof(int));

    cartItem item;
    printf("Please provide the productId and quantity to update the product in cart\n");
    scanf("%d%d",&item.productId,&item.quantity);

    write(socketDescriptor,&item,sizeof(cartItem));
    int res;
    read(socketDescriptor,&res,sizeof(int));

    if(!res)
        printf("Product does not exist in cart or sufficient quantity not availble or provided invalid data\n");
    else
        printf("Product with id: %d update in cart sucessfully\n",item.productId);
}

void RemoveProductFromCart(int socketDescriptor)
{
    int choice = 5;
    write(socketDescriptor,&choice,sizeof(int));

    cartItem item;
    printf("Please provide the productId to remove the product from cart\n");
    scanf("%d",&item.productId);

    write(socketDescriptor,&item,sizeof(cartItem));
    //to get the result of the operation
    int res;
    read(socketDescriptor,&res,sizeof(int));

    if(!res)
        printf("Product does not exist in cart\n");
    else
        printf("Product with id: %d removed from cart sucessfully\n",item.productId);
}

void PurchaseCart(int socketDescriptor)
{
    int choice = 6;
    write(socketDescriptor,&choice,sizeof(int));


    customer customer;
    read(socketDescriptor,&customer,sizeof(customer));
    
    int productCount;
    read(socketDescriptor,&productCount,sizeof(int));

    if(productCount==0)
    {
        printf("No items in cart\n");
        return;
    } 

    int receiptNo;
    read(socketDescriptor,&receiptNo,sizeof(int));
    product purchase[productCount];
    read(socketDescriptor,purchase,productCount*sizeof(product));
    
    char r[10];
    sprintf(r,"%d",receiptNo);
    strcat(r,".txt");

    int fd = creat(r,O_WRONLY|0744);

    char str[200];
    int totalBill=0,cost;
    for(int i=0;i<productCount;i++)
    {
        cost = (purchase[i].productPrice)*(purchase[i].productQuantity);
        totalBill += cost;
        sprintf(str,"%d. Product Name: %s, Price: %d, Quantity: %d, Cost: %d\n",i+1,purchase[i].productName,purchase[i].productPrice,purchase[i].productQuantity,cost);
        printf("%s",str);
    }
    sprintf(str,"Total Bill- %d\n",totalBill);
    printf("%s",str);
    int customerPay;
    printf("Please enter the payment\n");
    scanf("%d",&customerPay);
    printf("Receipt generated! reciept number- %d\n",receiptNo);
    sprintf(str,"Receipt Number- %d\nCustomer Email: %s\nCustomer Name: %s\n",receiptNo,customer.details.emailaddress,customer.details.name);
    write(fd,str,strlen(str));
    for(int i=0;i<productCount;i++)
    {
        cost = (purchase[i].productPrice)*(purchase[i].productQuantity);
        sprintf(str,"%d. Product Name: %s, Price: %d, Quantity: %d, Cost: %d\n",i+1,purchase[i].productName,purchase[i].productPrice,purchase[i].productQuantity,cost);
        write(fd,str,strlen(str));
    }
    sprintf(str,"Total Bill- %d\n",totalBill);
    write(fd,str,strlen(str));
    close(fd);
}