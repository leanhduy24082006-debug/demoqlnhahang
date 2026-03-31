#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <sstream>

using namespace std;

// Class for Menu Item
class MenuItem {
public:
    string name;
    double price;
    int quantity; // for inventory

    MenuItem(string n, double p, int q) : name(n), price(p), quantity(q) {}
};

// Class for Order
class Order {
public:
    vector<pair<MenuItem*, int>> items; // item and quantity ordered

    void addItem(MenuItem* item, int qty) {
        if (item->quantity >= qty) {
            items.push_back({item, qty});
            item->quantity -= qty;
        } else {
            cout << "Not enough stock for " << item->name << endl;
        }
    }

    double calculateTotal() {
        double total = 0;
        for (auto& p : items) {
            total += p.first->price * p.second;
        }
        return total;
    }

    void displayOrder() {
        cout << "Order Details:" << endl;
        for (auto& p : items) {
            cout << p.first->name << " x" << p.second << " = $" << fixed << setprecision(2) << p.first->price * p.second << endl;
        }
        cout << "Total: $" << fixed << setprecision(2) << calculateTotal() << endl;
    }
};

// Class for Bill/Invoice
class Bill {
private:
    static int billCount;
    int billID;
    vector<pair<string, pair<double, int>>> billItems; // name, price, quantity
    double subtotal;
    double taxRate;
    string date;

    string getCurrentDate() {
        time_t now = time(0);
        tm* ltm = localtime(&now);
        stringstream ss;
        ss << (1900 + ltm->tm_year) << "-" 
           << (1 + ltm->tm_mon) << "-" 
           << ltm->tm_mday << " "
           << ltm->tm_hour << ":" 
           << ltm->tm_min << ":" 
           << ltm->tm_sec;
        return ss.str();
    }

public:
    Bill() : billID(++billCount), subtotal(0), taxRate(0.1), date(getCurrentDate()) {}

    void addItem(string name, double price, int quantity) {
        billItems.push_back({name, {price, quantity}});
        subtotal += price * quantity;
    }

    void setTaxRate(double rate) {
        taxRate = rate;
    }

    double getSubtotal() const {
        return subtotal;
    }

    double getTax() const {
        return subtotal * taxRate;
    }

    double getTotal() const {
        return subtotal + getTax();
    }

    void displayBill() {
        cout << "\n" << string(50, "=") << endl;
        cout << setw(50) << "HÓA ĐƠN / BILL" << endl;
        cout << string(50, "=") << endl;
        cout << "Bill ID: " << billID << endl;
        cout << "Ngày/Date: " << date << endl;
        cout << string(50, "-") << endl;
        cout << left << setw(25) << "Tên/Item" 
             << setw(10) << "SL/Qty" 
             << setw(15) << "Giá/Price" << endl;
        cout << string(50, "-") << endl;

        for (auto& item : billItems) {
            cout << left << setw(25) << item.first
                 << setw(10) << item.second.second
                 << "$" << fixed << setprecision(2) << setw(13) << item.second.first * item.second.second << endl;
        }

        cout << string(50, "-") << endl;
        cout << right << setw(40) << "Tổng cộng/Subtotal: $" << fixed << setprecision(2) << subtotal << endl;
        cout << right << setw(40) << "Thuế 10%/Tax 10%: $" << fixed << setprecision(2) << getTax() << endl;
        cout << string(50, "=") << endl;
        cout << right << setw(40) << "TỔNG CỘNG/TOTAL: $" << fixed << setprecision(2) << getTotal() << endl;
        cout << string(50, "=") << endl << endl;
    }

    void saveBillToFile(const string& filename = "bill.txt") {
        ofstream outfile(filename, ios::app);
        if (!outfile.is_open()) {
            cout << "Lỗi: Không thể mở file!" << endl;
            return;
        }

        outfile << string(50, "=") << endl;
        outfile << setw(50) << "HÓA ĐƠN / BILL" << endl;
        outfile << string(50, "=") << endl;
        outfile << "Bill ID: " << billID << endl;
        outfile << "Ngày/Date: " << date << endl;
        outfile << string(50, "-") << endl;
        outfile << left << setw(25) << "Tên/Item" 
                << setw(10) << "SL/Qty" 
                << setw(15) << "Giá/Price" << endl;
        outfile << string(50, "-") << endl;

        for (auto& item : billItems) {
            outfile << left << setw(25) << item.first
                    << setw(10) << item.second.second
                    << "$" << fixed << setprecision(2) << setw(13) << item.second.first * item.second.second << endl;
        }

        outfile << string(50, "-") << endl;
        outfile << right << setw(40) << "Tổng cộng/Subtotal: $" << fixed << setprecision(2) << subtotal << endl;
        outfile << right << setw(40) << "Thuế 10%/Tax 10%: $" << fixed << setprecision(2) << getTax() << endl;
        outfile << string(50, "=") << endl;
        outfile << right << setw(40) << "TỔNG CỘNG/TOTAL: $" << fixed << setprecision(2) << getTotal() << endl;
        outfile << string(50, "=") << endl << endl;

        outfile.close();
        cout << "Hóa đơn đã được lưu vào file: " << filename << endl;
    }

    int getBillID() const {
        return billID;
    }
};

int Bill::billCount = 0;

// Class for Table
class Table {
public:
    int tableID;
    Order order;
    bool isOccupied;
    string customerName;

    Table(int id) : tableID(id), isOccupied(false), customerName("") {}

    void startOrder(string name) {
        isOccupied = true;
        customerName = name;
        order = Order();
    }

    void displayTableStatus() {
        cout << "Bàn/Table #" << tableID 
             << " - " << (isOccupied ? "Có khách" : "Trống")
             << (isOccupied ? " (Customer: " + customerName + ")" : "") << endl;
    }

    void endOrder() {
        isOccupied = false;
        customerName = "";
        order = Order();
    }
};

// Class for Restaurant
class Restaurant {
public:
    vector<MenuItem> menu;
    vector<Table> tables;

    Restaurant() {
        // Initialize menu with 15 items
        // Mains
        menu.push_back(MenuItem("Burger", 5.99, 15));
        menu.push_back(MenuItem("Pizza Large", 12.99, 10));
        menu.push_back(MenuItem("Pizza Small", 7.99, 15));
        menu.push_back(MenuItem("Pasta Carbonara", 8.99, 12));
        menu.push_back(MenuItem("Pasta Bolognese", 8.50, 12));
        menu.push_back(MenuItem("Grilled Chicken", 9.99, 10));
        menu.push_back(MenuItem("Steak", 14.99, 8));
        menu.push_back(MenuItem("Fish & Chips", 10.99, 10));
        
        // Sides & Appetizers
        menu.push_back(MenuItem("Fried Chicken Wings", 6.99, 20));
        menu.push_back(MenuItem("Spring Rolls", 4.99, 15));
        menu.push_back(MenuItem("Salad", 5.49, 15));
        menu.push_back(MenuItem("French Fries", 3.99, 20));
        
        // Drinks
        menu.push_back(MenuItem("Soft Drink", 2.99, 30));
        menu.push_back(MenuItem("Coffee", 3.50, 25));
        menu.push_back(MenuItem("Water", 1.50, 50));
        
        // Initialize 10 tables
        for (int i = 1; i <= 10; i++) {
            tables.push_back(Table(i));
        }
    }

    void displayMenu() {
        cout << "Menu:" << endl;
        for (int i = 0; i < menu.size(); ++i) {
            cout << i+1 << ". " << menu[i].name << " - $" << fixed << setprecision(2) << menu[i].price << " (Stock: " << menu[i].quantity << ")" << endl;
        }
    }

    MenuItem* getItem(int index) {
        if (index >= 0 && index < menu.size()) {
            return &menu[index];
        }
        return nullptr;
    }

    void displayAllTables() {
        cout << "\n=== TRẠNG THÁI BÀNG / TABLE STATUS ===" << endl;
        for (auto& table : tables) {
            table.displayTableStatus();
        }
        cout << "====================================\n" << endl;
    }

    Table* getTable(int id) {
        if (id > 0 && id <= tables.size()) {
            return &tables[id - 1];
        }
        return nullptr;
    }
};

int main() {
    Restaurant restaurant;
    int choice;

    cout << "=== Welcome to Restaurant Management System ===" << endl;

    while (true) {
        cout << "\n--- MENU CHÍNH / MAIN MENU ---" << endl;
        cout << "1. Display Menu" << endl;
        cout << "2. Order for Table (Oder cho bàn)" << endl;
        cout << "3. View All Tables" << endl;
        cout << "4. Calculate Bill for Table (Tính tiền bàn)" << endl;
        cout << "5. Quick Calculate Bill (Tính tiền nhanh 🧾)" << endl;
        cout << "6. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            restaurant.displayMenu();
        } 
        else if (choice == 2) {
            restaurant.displayAllTables();
            
            cout << "Chọn bàn / Select Table (1-10): ";
            int tableNum;
            cin >> tableNum;
            cin.ignore(); // clear newline
            
            Table* table = restaurant.getTable(tableNum);
            if (!table) {
                cout << "Invalid table number!" << endl;
                continue;
            }

            cout << "Nhập tên khách hàng / Customer name: ";
            string customerName;
            getline(cin, customerName);
            
            table->startOrder(customerName);
            cout << "Start order for Bàn " << tableNum << " - " << customerName << endl;

            // Order loop for this table
            bool continuseOrdering = true;
            while (continuseOrdering) {
                cout << "\nBàn " << tableNum << " - " << (table->isOccupied ? "Đang phục vụ" : "Trống") << endl;
                cout << "1. Add Item to Order" << endl;
                cout << "2. View Order" << endl;
                cout << "3. Calculate Bill & Finish 💰" << endl;
                cout << "4. View Bill & Ask to Pay 🧾" << endl;
                cout << "Choose: ";
                int orderChoice;
                cin >> orderChoice;

                if (orderChoice == 1) {
                    restaurant.displayMenu();
                    cout << "Enter item number: ";
                    int itemNum;
                    cin >> itemNum;
                    MenuItem* item = restaurant.getItem(itemNum - 1);
                    if (item) {
                        cout << "Enter quantity: ";
                        int qty;
                        cin >> qty;
                        table->order.addItem(item, qty);
                        cout << "✓ Thêm thành công! Item added!" << endl;
                        
                        // Quick options after adding item
                        cout << "\n--- HÀNH ĐỘNG TIẾP / QUICK ACTION ---" << endl;
                        cout << "Đơn hàng hiện tại / Current Order:" << endl;
                        table->order.displayOrder();
                        cout << "\n[1] Thêm item khác / Add More Items" << endl;
                        cout << "[2] Tính tiền & Xuất Hóa Đơn / Calculate Bill 💰" << endl;
                        cout << "[3] Xem Hóa Đơn / View Bill 🧾" << endl;
                        cout << "[4] Tiếp tục ""chọn bàn / Back to Main" << endl;
                        cout << "Chọn: ";
                        int quickChoice;
                        cin >> quickChoice;
                        
                        if (quickChoice == 1) {
                            // Continue to add more items - loop from top
                            continue;
                        } else if (quickChoice == 2) {
                            // Calculate Bill & Finish
                            if (table->order.items.empty()) {
                                cout << "❌ Order is empty!" << endl;
                                continue;
                            }

                            // Create bill from order
                            Bill bill;
                            for (auto& p : table->order.items) {
                                bill.addItem(p.first->name, p.first->price, p.second);
                            }

                            // Display bill
                            cout << "\n========== HÓA ĐƠN BÀNG " << tableNum << " ==========" << endl;
                            cout << "Khách hàng / Customer: " << table->customerName << endl;
                            bill.displayBill();

                            // Ask to save bill
                            cout << "Bạn có muốn lưu hóa đơn không? (Y/N): ";
                            char saveBill;
                            cin >> saveBill;
                            if (saveBill == 'Y' || saveBill == 'y') {
                                bill.saveBillToFile("bills.txt");
                            }

                            cout << "✓ Thank you! Cảm ơn khách hàng!" << endl;
                            table->endOrder();
                            continuseOrdering = false;
                        } else if (quickChoice == 3) {
                            // View Bill & Ask to Pay (không kết thúc order)
                            if (table->order.items.empty()) {
                                cout << "❌ Order is empty!" << endl;
                                continue;
                            }

                            // Create bill from order
                            Bill bill;
                            for (auto& p : table->order.items) {
                                bill.addItem(p.first->name, p.first->price, p.second);
                            }

                            // Display bill
                            cout << "\n" << string(50, "=") << endl;
                            cout << "========== HÓA ĐƠN / BILL - BÀN " << tableNum << " ==========" << endl;
                            cout << "Khách hàng / Customer: " << table->customerName << endl;
                            bill.displayBill();

                            cout << "💬 Khách hàng có muốn thanh toán tiếp không? / Customer Pay? (Y/N): ";
                            char isPay;
                            cin >> isPay;
                            
                            if (isPay == 'Y' || isPay == 'y') {
                                cout << "Bạn có muốn lưu hóa đơn không? (Y/N): ";
                                char saveBill;
                                cin >> saveBill;
                                if (saveBill == 'Y' || saveBill == 'y') {
                                    bill.saveBillToFile("bills.txt");
                                }
                                cout << "✓ Cảm ơn! / Thank you!" << endl;
                                table->endOrder();
                                continuseOrdering = false;
                            } else {
                                cout << "👍 Khách hàng tiếp tục / Customer continue..." << endl;
                            }
                        } else {
                            // quickChoice == 4 or invalid - go back to loop
                            continue;
                        }
                    } else {
                        cout << "Invalid item number." << endl;
                    }
                } 
                else if (orderChoice == 2) {
                    cout << "\n--- Order for Bàn " << tableNum << " ---" << endl;
                    table->order.displayOrder();
                } 
                else if (orderChoice == 3) {
                    if (table->order.items.empty()) {
                        cout << "❌ Order is empty!" << endl;
                        continue;
                    }

                    // Create bill from order
                    Bill bill;
                    for (auto& p : table->order.items) {
                        bill.addItem(p.first->name, p.first->price, p.second);
                    }

                    // Display bill
                    cout << "\n========== HÓA ĐƠN BÀNG " << tableNum << " ==========" << endl;
                    cout << "Khách hàng / Customer: " << table->customerName << endl;
                    bill.displayBill();

                    // Ask to save bill
                    cout << "Bạn có muốn lưu hóa đơn không? (Y/N): ";
                    char saveBill;
                    cin >> saveBill;
                    if (saveBill == 'Y' || saveBill == 'y') {
                        bill.saveBillToFile("bills.txt");
                    }

                    cout << "✓ Thank you! Cảm ơn khách hàng!" << endl;
                    table->endOrder();
                    continuseOrdering = false;
                }
                else if (orderChoice == 4) {
                    // View Bill & Ask to Pay (không kết thúc order)
                    if (table->order.items.empty()) {
                        cout << "❌ Order is empty!" << endl;
                        continue;
                    }

                    // Create bill from order
                    Bill bill;
                    for (auto& p : table->order.items) {
                        bill.addItem(p.first->name, p.first->price, p.second);
                    }

                    // Display bill
                    cout << "\n" << string(50, "=") << endl;
                    cout << "========== HÓA ĐƠN / BILL - BÀN " << tableNum << " ==========" << endl;
                    cout << "Khách hàng / Customer: " << table->customerName << endl;
                    bill.displayBill();

                    cout << "💬 Khách hàng có muốn thanh toán tiếp không? / Customer Pay? (Y/N): ";
                    char isPay;
                    cin >> isPay;
                    
                    if (isPay == 'Y' || isPay == 'y') {
                        cout << "Bạn có muốn lưu hóa đơn không? (Y/N): ";
                        char saveBill;
                        cin >> saveBill;
                        if (saveBill == 'Y' || saveBill == 'y') {
                            bill.saveBillToFile("bills.txt");
                        }
                        cout << "✓ Cảm ơn! / Thank you!" << endl;
                        table->endOrder();
                        continuseOrdering = false;
                    } else {
                        cout << "👍 Khách hàng tiếp tục / Customer continue..." << endl;
                    }
                } 
                else {
                    cout << "Invalid choice!" << endl;
                }
            }
        } 
        else if (choice == 3) {
            restaurant.displayAllTables();
        } 
        else if (choice == 4) {
            restaurant.displayAllTables();
            cout << "Chọn bàn để tính tiền / Select Table: ";
            int tableNum;
            cin >> tableNum;
            
            Table* table = restaurant.getTable(tableNum);
            if (!table || !table->isOccupied) {
                cout << "Bàn trống hoặc không hợp lệ!" << endl;
                continue;
            }

            if (table->order.items.empty()) {
                cout << "Bàn này chưa có order!" << endl;
                continue;
            }

            // Create bill from order
            Bill bill;
            for (auto& p : table->order.items) {
                bill.addItem(p.first->name, p.first->price, p.second);
            }

            // Display bill
            cout << "\n========== HÓA ĐƠN BÀNG " << tableNum << " ==========" << endl;
            cout << "Khách hàng / Customer: " << table->customerName << endl;
            bill.displayBill();

            // Ask to save bill
            cout << "Lưu hóa đơn? (Y/N): ";
            char saveBill;
            cin >> saveBill;
            if (saveBill == 'Y' || saveBill == 'y') {
                bill.saveBillToFile("bills.txt");
            }

            table->endOrder();
        } 
        else if (choice == 5) {
            // Quick Calculate Bill
            restaurant.displayAllTables();
            cout << "\nChọn bàn để tính tiền nhanh / Quick Bill: ";
            int tableNum;
            cin >> tableNum;
            
            Table* table = restaurant.getTable(tableNum);
            if (!table || !table->isOccupied) {
                cout << "❌ Bàn trống hoặc không hợp lệ!" << endl;
                continue;
            }

            if (table->order.items.empty()) {
                cout << "❌ Bàn này chưa có order!" << endl;
                continue;
            }

            // Create bill from order
            Bill bill;
            for (auto& p : table->order.items) {
                bill.addItem(p.first->name, p.first->price, p.second);
            }

            // Display bill
            cout << "\n" << string(50, "🧾") << endl;
            cout << "========== HÓA ĐƠN NHANH / QUICK BILL ==========" << endl;
            cout << "Bàn / Table: " << tableNum << endl;
            cout << "Khách hàng / Customer: " << table->customerName << endl;
            cout << string(50, "=") << endl;
            bill.displayBill();

            // Ask to save bill
            cout << "Lưu hóa đơn? (Y/N): ";
            char saveBill;
            cin >> saveBill;
            if (saveBill == 'Y' || saveBill == 'y') {
                bill.saveBillToFile("bills.txt");
            }

            cout << "✓ Cảm ơn! / Thank you!" << endl;
            table->endOrder();
        }
        else if (choice == 6) {
            cout << "Goodbye!" << endl;
            break;
        } 
        else {
            cout << "Invalid choice." << endl;
        }
    }

    return 0;
}