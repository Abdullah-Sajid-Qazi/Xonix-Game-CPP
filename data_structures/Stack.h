// Xonix Game - DSA Project
// Custom Stack implementation using linked list (LIFO)
// Used for match history and friend requests

#pragma once
#include <iostream>
#include <string>
using namespace std;

struct node
{
    string data;
    node *next = nullptr;
};

// Stack using Linked List - for match history and friend requests
class Stack
{
public:
    node *head = nullptr;

    Stack() {}

    void push(string value)
    {
        node *newNode = new node;
        newNode->data = value;
        newNode->next = head;
        head = newNode;
    }

    string pop()
    {
        if (isEmpty())
            throw runtime_error("Stack underflow: Cannot pop from an empty stack");

        string value = head->data;
        node *temp = head;
        head = head->next;
        delete temp;
        return value;
    }

    string peek()
    {
        if (isEmpty())
            throw runtime_error("Stack is empty: Cannot peek");
        return head->data;
    }

    bool isEmpty()
    {
        return !head;
    }

    int size()
    {
        int count = 0;
        node *current = head;
        while (current != nullptr)
        {
            count++;
            current = current->next;
        }
        return count;
    }

    void display()
    {
        if (isEmpty())
        {
            cout << "Stack is empty!!" << endl;
            return;
        }

        cout << "Stack: ";
        node *current = head;
        while (current != nullptr)
        {
            cout << current->data;
            current = current->next;
            if (current != nullptr)
                cout << " -> ";
        }
        cout << endl;
    }

    bool search(string value)
    {
        if (isEmpty())
            return false;

        node *current = head;
        while (current != nullptr)
        {
            if (current->data == value)
                return true;
            current = current->next;
        }
        return false;
    }

    // Deep copy constructor
    Stack(const Stack &other)
    {
        head = nullptr;
        if (other.head == nullptr)
            return;
        
        node *current = other.head;
        while (current != nullptr)
        {
            push(current->data);
            current = current->next;
        }
    }

    // Remove specific value from stack (uses temp stack to preserve order)
    void remove(string target)
    {
        Stack tempStack;
        bool found = false;

        while (!isEmpty())
        {
            string value = pop();
            if (value == target && !found)
            {
                found = true;
                continue;
            }
            tempStack.push(value);
        }

        // Restore elements
        while (!tempStack.isEmpty())
            push(tempStack.pop());

        if (!found)
            cout << "Item " << target << " not found in stack.\n";
    }
};
