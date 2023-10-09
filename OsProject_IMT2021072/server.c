#include "server.h"

int Menu(int socketDescriptor,int currUserId)
{
    int option;
    read(socketDescriptor,&option,sizeof(int));

    printf("got choice- %d\n",option);
    switch(option)
    {
        case 1:
            ListAllProducts(socketDescriptor);
            break;
        case 2:
            DisplayCartItems(socketDescriptor,currUserId);
            break;
        case 3:
            AddProductToCart(socketDescriptor,currUserId);
            break;
        case 4:
            UpdateProductInCart(socketDescriptor,currUserId);
            break;
        case 5:
            RemoveProductFromCart(socketDescriptor,currUserId);
            break;
        case 6:
            PurchaseCart(socketDescriptor,currUserId);
            break;
        case 7:
            AddProduct(socketDescriptor,currUserId);
            break;
        case 8:
            RemoveProduct(socketDescriptor,currUserId);
            break;
        case 9:
            UpdateProduct(socketDescriptor,currUserId);
            break;
        case 10:
            ListAllProducts(socketDescriptor);
            return 1;
            break;
        case 11:
            return 1;
        default:    
            break;
    }
    return 0;
}

int ValueCheck(product p)
{
    if(p.productQuantity<0 || p.productPrice<=0)
        return 0;
    return 1;
}

void receiptNumber(int socketDescriptor)
{
    int fd = open("recieptNumber.txt",O_RDWR,0744);

    int fl;
    struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=0;   
	lock.l_len=sizeof(int);	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return;
    }
    int receiptNo;

    read(fd,&receiptNo,sizeof(int));
    receiptNo++;

    lseek(fd,0,SEEK_SET);
    write(fd,&receiptNo,sizeof(int));
    printf("number- %d",receiptNo);
    
    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);

    write(socketDescriptor,&receiptNo,sizeof(int));
}

void PurchaseCart(int socketDescriptor,int currUserId)
{
    int fd = open("customers.txt",O_RDWR,0744);
    int fl;
    struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(currUserId-1)*sizeof(customer);   
	lock.l_len=sizeof(customer);	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return;
    }

    printf("before reading cust\n");
    customer cust;
    lseek(fd,(currUserId-1)*sizeof(customer),SEEK_SET);
    read(fd,&cust,sizeof(customer));//reading the customer record to read and modify the contents
    //sending customar record to give details to the client 
    write(socketDescriptor,&cust,sizeof(customer));


    printf("customer details: name= %s,email=%s,password=%s\n",cust.details.name,cust.details.emailaddress,cust.details.password);
    product p;
    int productCount=0;
    int cartSize = sizeof(cust.cart)/sizeof(cartItem);
    product allProducts[cartSize];
    for(int i=0;i<cartSize;i++)
    {
        if(cust.cart[i].quantity!=-1 && cust.cart[i].quantity!=0)
        {
            p = BuyProductById(cust.cart[i].productId,cust.cart[i].quantity);
            if(p.productQuantity!=-1)
            {
                allProducts[productCount]=p;
                productCount++;
            }
            cust.cart[i].quantity=-1;
            // printf("customer proId-%d qua-%d\n",cust.cart[i].productId,cust.cart[i].quantity);
        }
    }

    //writing the customer record after modifying it
    lseek(fd,(currUserId-1)*sizeof(customer),SEEK_SET);
    write(fd,&cust,sizeof(customer));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);

    write(socketDescriptor,&productCount,sizeof(int));
    if(productCount==0)
        return;

    receiptNumber(socketDescriptor);
    write(socketDescriptor,allProducts,productCount*sizeof(product));
}

product BuyProductById(int id,int quantity)
{

    int fd = open("products.txt",O_RDWR,0744);

    int fl;
    struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(id-1)*sizeof(product);   
	lock.l_len=sizeof(product);	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        product p;
        p.productQuantity=-1;
        return p;
    }
    
    product productPresent,bought;
    lseek(fd,(id-1)*sizeof(product),SEEK_SET);
    read(fd,&productPresent,sizeof(product));
    bought=productPresent;

    if(productPresent.productQuantity>quantity)
    {
        productPresent.productQuantity=productPresent.productQuantity-quantity;
        bought.productQuantity=quantity;
    }
    else if(productPresent.productQuantity!=-1)
    {
        productPresent.productQuantity=0;
    }

    lseek(fd,(productPresent.productId-1)*sizeof(product),SEEK_SET);
    write(fd,&productPresent,sizeof(product));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);

    return bought;
}

int UserMenu(int socketDescriptor)
{
    printf("got into UserMenu %d\n",getpid());
    int choice,result=0;
    customer user;int currUserId;
    read(socketDescriptor,&choice,sizeof(int));
    if(choice==4)
        return -1;
    read(socketDescriptor,&user,sizeof(customer));

    // printf("completed reading in Usermenu\n");
    // printf("%d %s %s\n",choice,user.details.emailaddress,user.details.password);
    switch(choice)
    {
        case 1:
            // printf("name:%s,email:%s,password:%s",user.details.name,user.details.emailaddress,user.details.password);
            currUserId = Register(socketDescriptor,user);
            if(currUserId)
            {
                result=1;
                write(socketDescriptor,&result,sizeof(int));
                Menu(socketDescriptor,currUserId);
                return currUserId;
            }
            else
            {
                write(socketDescriptor,&result,sizeof(int));
                UserMenu(socketDescriptor);
            }
            break;
        //Validating Customer or Admin
        case 2:
        case 3:
            printf("entered Validate\n");
            currUserId = ValidateUser(socketDescriptor,user,choice);
            if(currUserId)
            {
                result=1;
                write(socketDescriptor,&result,sizeof(int));
                Menu(socketDescriptor,currUserId);
                return currUserId;
            }
            else
            {
                write(socketDescriptor,&result,sizeof(int));
                UserMenu(socketDescriptor);
            }
            break;
    }
}

void updateCarts(int productId,int quantity)
{
    int fd = open("customers.txt",O_RDWR,0744);

    int fl;
    struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=0;   
	lock.l_len=0;	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return;
    }

    customer updateCustomer;
    while(read(fd,&updateCustomer,sizeof(customer)))
    {
        //checking if the product is present to update, if so updating it
        int cartSize = sizeof(updateCustomer.cart)/sizeof(cartItem);
        for(int i=0;i<cartSize;i++)
        {
            if(updateCustomer.cart[i].productId==productId && updateCustomer.cart[i].quantity>quantity)
                updateCustomer.cart[i].quantity=quantity;
        }
        lseek(fd,-1*sizeof(customer),SEEK_CUR);
        write(fd,&updateCustomer,sizeof(customer));
    }

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
}

void UpdateProduct(int socketDescriptor,int currUserId)
{
    product userProduct;
    read(socketDescriptor,&userProduct,sizeof(product));

    cartItem userItem;
    userItem.productId = userProduct.productId;
    userItem.quantity = 0;

    int result=0;
    if(!ProductCheck(userItem)  || !ValueCheck(userProduct))
    {
        write(socketDescriptor,&result,sizeof(int));
        // printf("product does not exists");
        return;
    }

    int fd = open("products.txt",O_RDWR,0744);

    int fl;
    struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(userProduct.productId-1)*sizeof(product);   
	lock.l_len=sizeof(product);	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return;
    }
    
    result=1;
    product productPresent;
    lseek(fd,(userProduct.productId-1)*sizeof(product),SEEK_SET);
    read(fd,&productPresent,sizeof(product));
    productPresent.productQuantity=userProduct.productQuantity;
    productPresent.productPrice=userProduct.productPrice;

    lseek(fd,(productPresent.productId-1)*sizeof(product),SEEK_SET);
    write(fd,&productPresent,sizeof(product));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);

    // printf("result- %d",result);
    write(socketDescriptor,&result,sizeof(int));
    
    // updateCarts(userProduct.productId,userProduct.productQuantity);
}


void RemoveProduct(int socketDescriptor,int currUserId)
{
    product userProduct;
    read(socketDescriptor,&userProduct,sizeof(product));

    cartItem userItem;
    userItem.productId = userProduct.productId;
    userItem.quantity = 0;

    int result=0;
    if(!ProductCheck(userItem))
    {
        write(socketDescriptor,&result,sizeof(int));
        return;
    }

    int fd = open("products.txt",O_RDWR,0744);

    int fl;
    struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(userProduct.productId-1)*sizeof(product);   
	lock.l_len=sizeof(product);	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return;
    }
    
    result=1;
    userProduct.productQuantity=-1;
    lseek(fd,(userProduct.productId-1)*sizeof(product),SEEK_SET);
    write(fd,&userProduct,sizeof(product));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);

    write(socketDescriptor,&result,sizeof(int));
    
    // updateCarts(userProduct.productId,-1);
}

void AddProduct(int socketDescriptor,int currUserId)
{
    product userProduct;
    read(socketDescriptor,&userProduct,sizeof(product));

    int result=0;
    if(!ValueCheck(userProduct))
    {
        write(socketDescriptor,&result,sizeof(int));
        // printf("product does not exists");
        return;
    }
    
    int fd = open("products.txt",O_RDWR,0744);

    int fl;
    struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=0;   
	lock.l_len=0;	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return;
    }

    product checkProduct;
    while(read(fd,&checkProduct,sizeof(product)))
    {
        if(checkProduct.productQuantity==-1)
        {
            userProduct.productId = checkProduct.productId;
            result=checkProduct.productId;
            printf("productId- %d\n",result);
            break;
        }
        else
        {
            printf("proName- %s\n",checkProduct.productName);
            // printf("%d %s %d %d\n",checkProduct.productId,checkProduct.productName,checkProduct.productQuantity,checkProduct.productPrice);
        }
    }

    lseek(fd,(userProduct.productId-1)*sizeof(product),SEEK_SET);
    write(fd,&userProduct,sizeof(product));

    write(socketDescriptor,&result,sizeof(int));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
}



void UpdateProductInCart(int socketDescriptor,int currUserId)
{
    printf("entered updateCart\n");
    cartItem userItem;
    read(socketDescriptor,&userItem,sizeof(userItem));
    printf("userItem- %d %d\n",userItem.productId,userItem.quantity);
    product p;
    p.productQuantity=userItem.quantity;
    p.productPrice=1;
    int result=0;
    if(!ProductCheck(userItem) || !ValueCheck(p))//checking if the product exists and has sufficient quantity to update the cart Item
    {
        write(socketDescriptor,&result,sizeof(int));
        return;
    }

    printf("crossed check\n");
    int fd = open("customers.txt",O_RDWR,0744);
    printf("opened file\n");
    int fl;
    struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(currUserId-1)*sizeof(customer);   
	lock.l_len=sizeof(customer);	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return;
    }

    printf("before reading cust\n");
    customer cust;
    lseek(fd,(currUserId-1)*sizeof(customer),SEEK_SET);
    read(fd,&cust,sizeof(customer));//reading the customer record to read and modify the contents
    printf("customer details: name= %s,email=%s,password=%s\n",cust.details.name,cust.details.emailaddress,cust.details.password);
    //checking if the product is present to update, if so updating it
    int cartSize = sizeof(cust.cart)/sizeof(cartItem);
    result=0;
    for(int i=0;i<cartSize;i++)
    {
        if(cust.cart[i].quantity!=-1 && cust.cart[i].productId==userItem.productId)
        {
            result=1;
            cust.cart[i]=userItem;
            printf("customer proId-%d qua-%d\n",cust.cart[i].productId,cust.cart[i].quantity);
            break;
        }
    }

    write(socketDescriptor,&result,sizeof(int));

    //writing the customer record after modifying it
    lseek(fd,(currUserId-1)*sizeof(customer),SEEK_SET);
    write(fd,&cust,sizeof(customer));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
}

void RemoveProductFromCart(int socketDescriptor,int currUserId)
{
    cartItem userItem;
    read(socketDescriptor,&userItem,sizeof(cartItem));

    userItem.quantity=-1;

    int result=0;
    if(!ProductCheck(userItem))//checking if the product exists to remove it
    {
        write(socketDescriptor,&result,sizeof(result));
        return;
    }

    int fd = open("customers.txt",O_RDWR,0744);
    int fl;
    struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(currUserId-1)*sizeof(customer);   
	lock.l_len=sizeof(customer);	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return;
    }

    customer cust;
    lseek(fd,(currUserId-1)*sizeof(customer),SEEK_SET);
    read(fd,&cust,sizeof(customer));//reading the customer record to read and modify the contents

    //checking if the product to remove is present, if present removing it from the cart
    int cartSize = sizeof(cust.cart)/sizeof(cartItem);
    for(int i=0;i<cartSize;i++)
    {
        if(cust.cart[i].quantity!=-1 && cust.cart[i].productId==userItem.productId)
        {
            result=1;
            cust.cart[i].quantity=-1;
            break;
        }
    }

    write(socketDescriptor,&result,sizeof(int));

    //writing the customer record after modifying it
    lseek(fd,(currUserId-1)*sizeof(customer),SEEK_SET);
    write(fd,&cust,sizeof(customer));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
}

int ProductCheck(cartItem userItem)
{
    int fd = open("products.txt",O_RDONLY,0744);
    int fl;
    struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(userItem.productId-1)*sizeof(product);   
	lock.l_len=sizeof(product);	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return 0;
    }

    int valid=1;
    product checkProduct;
    lseek(fd,(userItem.productId-1)*sizeof(product),SEEK_SET);
    read(fd,&checkProduct,sizeof(product));
    if(checkProduct.productQuantity==-1 || checkProduct.productQuantity<userItem.quantity)
        valid=0;

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);

    return valid;
}

void AddProductToCart(int socketDescriptor,int currUserId)
{
    cartItem userItem;
    read(socketDescriptor,&userItem,sizeof(userItem));

    int result=0;
    if(!ProductCheck(userItem))//checking if the product exists and has sufficient quantity
    {
        write(socketDescriptor,&result,sizeof(result));
        return;
    }

    if(userItem.quantity<=0)
    {
        write(socketDescriptor,&result,sizeof(result));
        return;
    }

    int fd = open("customers.txt",O_RDWR,0744);
    int fl;
    struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(currUserId-1)*sizeof(customer);   
	lock.l_len=sizeof(customer);	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return;
    }

    customer cust;
    lseek(fd,(currUserId-1)*sizeof(customer),SEEK_SET);
    read(fd,&cust,sizeof(customer));//reading the customer record to read and modify the contents

    //checking if the product is already present, if so can not add
    int cartSize = sizeof(cust.cart)/sizeof(cartItem);
    result=1;
    for(int i=0;i<cartSize;i++)
    {
        if(cust.cart[i].quantity!=-1 && cust.cart[i].productId==userItem.productId)
            result=0;
    }

    int added=0;//to represent if product added sucessfully
    if(result)//If not present try adding to the cart
    {
        for(int i=0;i<cartSize;i++)
        {
            if(cust.cart[i].quantity==-1)
            {
                cust.cart[i]=userItem;
                added=1;
                break;
            }
        }
    }

    write(socketDescriptor,&added,sizeof(int));

    //writing the customer record after modifying it
    lseek(fd,(currUserId-1)*sizeof(customer),SEEK_SET);
    write(fd,&cust,sizeof(customer));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
}

void DisplayCartItems(int socketDescriptor,int currUserId)
{
    // printf("entered cart - %d\n",currUserId);
    int fd = open("customers.txt",O_RDONLY,0744);
    int fl;
    struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(currUserId-1)*sizeof(customer);   
	lock.l_len=sizeof(customer);	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        printf("display fcntl\n");
        return;
    }

    // printf("before read cust\n");
    customer cust;
    lseek(fd,(currUserId-1)*sizeof(customer),SEEK_SET);
    read(fd,&cust,sizeof(customer));

    // printf("customer details: name= %s,email=%s,password=%s\n",cust.details.name,cust.details.emailaddress,cust.details.password);
    // for(int i=0;i<5;i++)
    //     printf("%d %d\n",cust.cart[i].productId,cust.cart[i].quantity);
    // write(socketDescriptor,&cust,sizeof(customer));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);

    product cartProducts[50];
    int cartCount=0;
    int cartSize = sizeof(cust.cart)/sizeof(cartItem);
    for(int i=0;i<cartSize;i++)
    {
        if(cust.cart[i].quantity!=-1 && cust.cart[i].quantity!=0)
        {
            // printf("%d %d\n",cust.cart[i].productId,cust.cart[i].quantity);
            cartProducts[cartCount]=ProductById(cust.cart[i].productId);
            if(cartProducts[cartCount].productQuantity>cust.cart[i].quantity)
                cartProducts[cartCount].productQuantity=cust.cart[i].quantity;
            cust.cart[i].quantity=cartProducts[cartCount].productQuantity;
            cartCount++;
            // printf("%d %d\n",cust.cart[i].productId,cust.cart[i].quantity);
        }
    }

    write(socketDescriptor,&cartCount,sizeof(int));//number of products in cart
    write(socketDescriptor,cartProducts,sizeof(product)*(cartCount));//products in cart

    //updating cart:

    fd = open("customers.txt",O_RDONLY,0744);
	lock.l_type = F_RDLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(currUserId-1)*sizeof(customer);   
	lock.l_len=sizeof(customer);	           
	lock.l_pid=getpid();

    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        printf("display fcntl\n");
        return;
    }

    // printf("before read cust\n");
    lseek(fd,(currUserId-1)*sizeof(customer),SEEK_SET);
    write(fd,&cust,sizeof(customer));

    lock.l_type = F_UNLCK;
    fcntl(fd,F_SETLKW,&lock);
}

int Register(int socketDescriptor,customer user)
{
    customer customer;
    int fd = open("customers.txt",O_RDWR);
    if(fd==-1)
    {
        write(1,"Register fd\n",13);
        perror("open");
    }
    int fl;
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=0;   
	lock.l_len=0;	           
	lock.l_pid=getpid();
    
    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        write(1,"Register fcntl",15);
        perror("fcntl");
        return 1;
    }

    int id=0;
    //Checking if the user exists already
    while(read(fd,&customer,sizeof(customer)))
        if(!strcmp(customer.details.emailaddress,user.details.emailaddress))
        {
            id=-1;
            // write(1,&user.details.emailaddress,sizeof(user.details.emailaddress));
        }

    char str[200];
    // sprintf(str, "Value of id = %d",id);
    // write(1,str,40);
    // read(fd,&customer,sizeof(customer));
    lseek(fd,0,SEEK_SET);
    //Registering only if user already not present
    if(id!=-1)
    {
        // sprintf(str, "Value of if id = %d",id);
        // write(1,str,20);
        while(read(fd,&customer,sizeof(customer)))
        {
            // write(1,"entered while",14);
            // sprintf(str,"customer %s %s %s",customer.details.name,customer.details.emailaddress,customer.details.password);
            // write(1,str,40);
            if(customer.assigned==0)
            {
                // sprintf(str, "Value of id = %d",customer.customerId);
                // write(1,str,40);
                strcpy(customer.details.name,user.details.name);
                strcpy(customer.details.emailaddress,user.details.emailaddress);
                strcpy(customer.details.password,user.details.password);
                // sprintf(str,"customer %s %s %s",customer.details.name,customer.details.emailaddress,customer.details.password);
                // write(1,str,40);
                // sprintf(str,"customer %s",user.details.name);
                // write(1,str,70);
                customer.assigned=1;
                id = customer.customerId;
                lseek(fd,-1*sizeof(customer),SEEK_CUR);
                write(fd,&customer,sizeof(customer));
                break;
            }
        }
        // lseek(fd,-1*sizeof(customer),SEEK_CUR);
        // read(fd,&customer,sizeof(customer));
        // sprintf(str,"customer %s %s %s",customer.details.name,customer.details.emailaddress,customer.details.password);
        // write(1,str,40);
    }
    else
        id=0;
    write(1,"notentered",11);
    sprintf(str, "Value of id = %d",id);
    write(1,str,40);
    lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
    return id;
}

int ValidateUser(int socketDescriptor,customer user,int type)
{
    customer customer;int fd;
    if(type==2)
        fd = open("customers.txt",O_RDONLY);
    else if(type==3)
    {
        person admin;
        strcpy(admin.emailaddress,"admin");
        strcpy(admin.password,"pass");
        // printf("entered validate admin\n");
        if(!strcmp(user.details.emailaddress,admin.emailaddress) && !strcmp(user.details.password,admin.password))
            return 100;
        else
            return 0;
    }

    int fl;
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=0;   
	lock.l_len=0;	           
	lock.l_pid=getpid();
    
    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return 1;
    }

    char str[200];
    lseek(fd,0,SEEK_SET);
    int valid=0;

    // printf("email-%s passwrd-%s\n",user.details.emailaddress,user.details.password);

    // sprintf(str,"\nuser- %s\t",user.details.emailaddress);
    // write(1,str,12);

    // sprintf(str,"\nuser- %s\t",user.details.password);
    // write(1,str,12);
    while(read(fd,&customer,sizeof(customer)))
    {
        // if(customer.assigned==1)
        // {
        //     // sprintf(str,"user- %s %s\n",customer.details.emailaddress,customer.details.password);
        //     // write(1,str,15);
        //     printf("email-%s passowrd-%s\n",customer.details.emailaddress,customer.details.password);
        //     if(!strcmp(customer.details.emailaddress,user.details.emailaddress))
        //         printf("match found\n");
        //     else
        //         printf("%d\n %s %s",strcmp(customer.details.emailaddress,user.details.emailaddress),customer.details.emailaddress,user.details.emailaddress);
        // }
        if(!strcmp(customer.details.emailaddress,user.details.emailaddress) && customer.assigned!=0)
        {
            // sprintf(str,"userPassword: %s",customer.details.password);
            // write(1,str,20);
            if(!strcmp(customer.details.password,user.details.password))
            {
                valid=customer.customerId;
                break;
            }
        }
    }

    lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);

    return valid;
}

void ListAllProducts(int socketDescriptor)
{
    int fd = open("products.txt",O_RDONLY,0744);
    int fl;
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=0;   
	lock.l_len=0;	           
	lock.l_pid=getpid();
    
    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        return;
    }

    int productCount=0;
    product allProducts[2000];
    product p;
    int valid=0;
    while(read(fd,&p,sizeof(product)))
        if(p.productQuantity != -1)
        {
            allProducts[productCount]=p;
            productCount++;
        }
    printf("count- %d\n",productCount);
    result count;
    count.res = productCount;
    printf("count.res- %d\n",count.res);
    int x = write(socketDescriptor,&count,sizeof(result));
    printf("bytes written- %d\n",x);
    // write(socketDescriptor,&productCount,sizeof(int));//sending the number of products availble
    write(socketDescriptor,allProducts,sizeof(product)*(productCount));//sending all the products

    lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);
    close(fd);
}

product ProductById(int id)
{
    int fd = open("products.txt",O_RDONLY,0744);
    if(fd==-1)
        perror("fd");
    int fl;
	struct flock lock;
	lock.l_type = F_RDLCK;
	lock.l_whence=SEEK_SET;
	lock.l_start=(id-1)*sizeof(product);   
	lock.l_len=sizeof(product);	           
	lock.l_pid=getpid();
    
    product p;
    fl = fcntl(fd,F_SETLKW,&lock);
    if(fl==-1)
    {
        perror("fcntl");
        printf("productById fcntl");
        return p;
    }
    lseek(fd,(id-1)*sizeof(product),SEEK_SET);
    read(fd,&p,sizeof(product));

    lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLK,&lock);

    return p;
}