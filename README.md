# SmartCart

## Overview

This project implements a server-client architecture for an Inventory Management System using socket programming in C. The system involves an Admin (server) and User (client) with communication facilitated through sockets. It provides fundamental functionalities for product management, cart handling, and payment processing. Concurrency control mechanisms, including locks, are implemented to ensure data integrity during critical operations.

## Problem Statement

### Admin Menu

1. **Add/Del a product:** Add or delete a product from the inventory.
2. **Update quantity/price:** Modify the quantity or price of an existing product. Generates a log file with stock levels and product details upon exit.

### User Menu

- **Display Products:** Lists all available products: P_ID, P_Name, Cost.
- **Manage Cart:** View, add products with quantities, and edit the cart.
- **Payment:** Initiate a payment process by entering the amount in the terminal. Generates a receipt log file upon completion.

### Additional Features

- **Concurrency Control:** Implements locks during critical operations, preventing conflicts during payment gateway access or admin product interaction.
- **Product Locks:** Ensures exclusive access to products in the cart and products updated by the admin.

## Project Structure
- OSProject_IMT2021072
  - report
  - all code files
- readme.md



## Instructions to Run

### Step 1: Clone the Repository
```bash
git clone https://github.com/RamsaiKoushik/SmartCart
cd OSProject_IMT2021072
```

### Step 2: Open Two Terminals
- Open two separate terminal windows or tabs.

### Step 3: Compile and Run the Server (Terminal 1)
- In the first terminal, run the following commands:

```bash
cc -o server main_server.c server.c
```

```bash
./server
```

### Step 4: Compile and Run the Client (Terminal 2)

- In the second terminal, run the following commands:

```bash
cc -o client main_client.c client.c
```
```bash
./client
```

Make sure to run the server first before starting the client.

