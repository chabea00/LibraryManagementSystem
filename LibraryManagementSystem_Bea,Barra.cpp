#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;


class Book
{
private:
    string title;
    string author;
    string isbn;
    bool isAvailable;

public:
    Book(string t = "", string a = "", string i = "", bool avail = true)
        : title(t), author(a), isbn(i), isAvailable(avail) {}

    
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getISBN() const { return isbn; }
    bool getAvailability() const { return isAvailable; }

    void setTitle(string t) { title = t; }
    void setAuthor(string a) { author = a; }
    void setISBN(string i) { isbn = i; }
    void setAvailability(bool avail) { isAvailable = avail; }

    void display() const 
    {
        cout << "Title: " << title
             << " | Author: " << author
             << " | ISBN: " << isbn
             << " | Status: " << (isAvailable ? "Available" : "Borrowed") << endl << endl;
    }
    
    string toFile() const 
    {
        return title + ";" + author + ";" + isbn + ";" + (isAvailable ? "1" : "0");
    }

    void fromFile(string line) 
    {
        size_t pos1 = line.find(';');
        size_t pos2 = line.find(';', pos1 + 1);
        size_t pos3 = line.find(';', pos2 + 1);

        title = line.substr(0, pos1);
        author = line.substr(pos1 + 1, pos2 - pos1 - 1);
        isbn = line.substr(pos2 + 1, pos3 - pos2 - 1);
        isAvailable = (line.substr(pos3 + 1) == "1");
    }
};

// library user
class LibraryUser 
{
private:
    string userID;
    string name;
    vector<string> borrowedBooks; 

public:
    LibraryUser(string id = "", string n = "") : userID(id), name(n) {}

    string getUserID() const { return userID; }
    string getName() const { return name; }
    vector<string> getBorrowedBooks() const { return borrowedBooks; }

    void setName(string n) { name = n; }

    void borrowBook(string isbn) { borrowedBooks.push_back(isbn); }
    void returnBook(string isbn) {
        borrowedBooks.erase(remove(borrowedBooks.begin(), borrowedBooks.end(), isbn), borrowedBooks.end());
    }

    void display() const 
    {
        cout << "User ID: " << userID << " | Name: " << name << " | Borrowed Books: ";
        if (borrowedBooks.empty())
            cout << "None" << endl;
        else {
            for (const auto &b : borrowedBooks)
                cout << b << " ";
        }
        cout << endl;
    }

    string toFile() const {
        string line = userID + ";" + name + ";";
        for (size_t i = 0; i < borrowedBooks.size(); i++) {
            line += borrowedBooks[i];
            if (i != borrowedBooks.size() - 1) line += ",";
        }
        return line;
    }

    void fromFile(string line) {
        size_t pos1 = line.find(';');
        size_t pos2 = line.find(';', pos1 + 1);

        userID = line.substr(0, pos1);
        name = line.substr(pos1 + 1, pos2 - pos1 - 1);

        borrowedBooks.clear();
        string borrowed = line.substr(pos2 + 1);
        size_t start = 0, end = borrowed.find(',');
        while (end != string::npos) {
            borrowedBooks.push_back(borrowed.substr(start, end - start));
            start = end + 1;
            end = borrowed.find(',', start);
        }
        if (!borrowed.empty())
            borrowedBooks.push_back(borrowed.substr(start));
    }
};


class Library 
{
private:
    vector<Book> books;
    vector<LibraryUser> users;

    const string booksFile = "books.txt";
    const string usersFile = "users.txt";

public:
    Library() { loadData(); }
    ~Library() { saveData(); }

    void addBook() {
        string title, author, isbn;
        cout << "Enter Title: ";
        cin.ignore();
        getline(cin, title);
        cout << "Enter Author: ";
        getline(cin, author);
        cout << "Enter ISBN: ";
        getline(cin, isbn);

        books.push_back(Book(title, author, isbn, true));
        cout << "BOOK ADDED SUCCESSFULLY!\n" << endl;

        saveData(); 
    }
    

    void removeBook() 
    {
        string isbn;
        cout << "Enter ISBN to remove: ";
        cin >> isbn;

        auto it = remove_if(books.begin(), books.end(),
                            [&](Book &b) { return b.getISBN() == isbn; });
        if (it != books.end()) {
            books.erase(it, books.end());
            cout << "BOOK REMOVED SUCCESSFULLY!\n" << endl;
        } else {
            cout << "BOOK NOT FOUND!\n" << endl;
        }
        
        saveData(); 
    }


    void displayBooks() const 
    {
    	cout << " -----------------------------------"<< endl;
        cout << "             Book List              "<< endl;
        cout << " -----------------------------------"<< endl << endl;
        for (const auto &b : books) b.display();
    }
    
    void registerUser() 
{
    string id, name;
    cout << "Enter user ID: ";
    cin >> id;
    cout << "Enter name: ";
    cin.ignore();
    getline(cin, name);

    users.push_back(LibraryUser(id, name));
    cout << "USER REGISTERED SUCCESSFULLY!\n" << endl;

    saveData(); 
}

void removeUser() 
{
    string id;
    cout << "Enter User ID to remove: ";
    cin >> id;

    auto it = remove_if(users.begin(), users.end(),
                        [&](LibraryUser &u) { return u.getUserID() == id; });
    if (it != users.end()) {
        users.erase(it, users.end());
        cout << "USER REMOVED SUCCESSFULLY!\n" << endl;
    } else {
        cout << "USER NOT FOUND!\n" << endl;
    }

    saveData();  
}


    void displayUsers() const 
    {
    	cout << " -----------------------------------"<< endl;
        cout << "             User List              "<< endl;
        cout << " -----------------------------------"<< endl << endl;
        for (const auto &u : users) u.display();
    }

void borrowBook() 
{
    string userID, isbn;
    cout << "Enter User ID: ";
    cin >> userID;
    cout << "Enter ISBN of the book: ";
    cin >> isbn;

    auto userIt = find_if(users.begin(), users.end(),
                          [&](LibraryUser &u) { return u.getUserID() == userID; });
    auto bookIt = find_if(books.begin(), books.end(),
                          [&](Book &b) { return b.getISBN() == isbn; });

    if (userIt != users.end() && bookIt != books.end()) {
        if (bookIt->getAvailability()) {
            bookIt->setAvailability(false);
            userIt->borrowBook(isbn);
            cout << "BOOK BORROWED SUCCESSFULLY!\n" << endl;
        } else {
            cout << "BOOK IS ALREADY BORROWED!\n" << endl;
        }
    } else {
        cout << "USER OR BOOK NOT FOUND!\n" << endl;
    }

    saveData(); 
}

void returnBook() 
{
    string userID, isbn;
    cout << "Enter User ID: ";
    cin >> userID;
    cout << "Enter ISBN of the book to return: ";
    cin >> isbn;

    auto userIt = find_if(users.begin(), users.end(),
                          [&](LibraryUser &u) { return u.getUserID() == userID; });
    auto bookIt = find_if(books.begin(), books.end(),
                          [&](Book &b) { return b.getISBN() == isbn; });

    if (userIt != users.end() && bookIt != books.end()) {
        bookIt->setAvailability(true);
        userIt->returnBook(isbn);
        cout << "BOOK RETURNED SUCCESSFULLY!\n" << endl;
    } else {
        cout << "USER OR BOOK NOT FOUND!\n" << endl;
    }

    saveData(); 
}

    void loadData()
    {
        ifstream bfile(booksFile);
        if (bfile.is_open()) {
            string line;
            while (getline(bfile, line)) {
                Book b;
                b.fromFile(line);
                books.push_back(b);
            }
            bfile.close();
        }

        ifstream ufile(usersFile);
        if (ufile.is_open()) {
            string line;
            while (getline(ufile, line)) {
                LibraryUser u;
                u.fromFile(line);
                users.push_back(u);
            }
            ufile.close();
        }
    }

    void saveData() 
    {
        ofstream bfile(booksFile);
        if (!bfile.is_open()) {
            cerr << "Failed to open books.txt for writing." << endl;
            return;
        }
        for (auto &b : books) bfile << b.toFile() << endl;
        bfile.close();

        ofstream ufile(usersFile);
        for (auto &u : users) ufile << u.toFile() << endl;
        ufile.close();
    }
};

int main() 
{
    Library library;
    int choice;

    do 
    {
        cout << " -----------------------------------"<< endl;
        cout << "       Library Management System    "<< endl;
        cout << " -----------------------------------"<< endl << endl;
        cout << "1. Add Book\n";
        cout << "2. Remove Book\n";
        cout << "3. Display Books\n";
        cout << "4. Register User\n";
        cout << "5. Remove User\n";
        cout << "6. Display Users\n";
        cout << "7. Borrow Book\n";
        cout << "8. Return Book\n";
        cout << "0. Exit\n" << endl;
        cout << "ENTER CHOICE: ";
        cin >> choice;

        switch (choice) 
        {
        case 1: library.addBook(); break;
        case 2: library.removeBook(); break;
        case 3: library.displayBooks(); break;
        case 4: library.registerUser(); break;
        case 5: library.removeUser(); break;
        case 6: library.displayUsers(); break;
        case 7: library.borrowBook(); break;
        case 8: library.returnBook(); break;
        case 0: cout << "THANK YOU!\n"; break;
        default: cout << "INVALID CHOICE!\n";
        }
    } while (choice != 0);

    return 0;
}