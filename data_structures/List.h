// Xonix Game - DSA Project
// Custom Linked List implementation for storing friends list and sequential data

#pragma once
#include <iostream>
#include <string>
using namespace std;

struct NodeList
{
    string data;
    NodeList *next = nullptr;
};

// Singly Linked List - used for friends list management
class LinkedList
{
public:
    NodeList *head = nullptr;

    // Insert at end of list
    void insert(string value)
    {
        NodeList *newNode = new NodeList;
        newNode->data = value;

        if (!head)
        {
            head = newNode;
        }
        else
        {
            NodeList *curr = head;
            while (curr->next)
                curr = curr->next;
            curr->next = newNode;
        }
    }

    // Remove first occurrence of value
    void deleteNode(string value)
    {
        if (head == nullptr)
        {
            cout << "List is empty.\n";
            return;
        }

        if (head->data == value)
        {
            NodeList *temp = head;
            head = head->next;
            delete temp;
            return;
        }

        NodeList *curr = head, *prev = nullptr;
        while (curr && curr->data != value)
        {
            prev = curr;
            curr = curr->next;
        }

        if (curr)
        {
            prev->next = curr->next;
            delete curr;
        }
    }

    bool isEmpty()
    {
        return !head;
    }

    // Remove and return first element
    string pop()
    {
        if (!head)
            throw runtime_error("Cannot pop from an empty list");
        
        NodeList *temp = head;
        head = head->next;
        string temp1 = temp->data;
        delete temp;
        return temp1;
    }

    bool search(string value)
    {
        NodeList *curr = head;
        while (curr != nullptr)
        {
            if (curr->data == value)
                return true;
            curr = curr->next;
        }
        return false;
    }

    void display()
    {
        NodeList *temp = head;
        while (temp != nullptr)
        {
            cout << temp->data << " -> ";
            temp = temp->next;
        }
        cout << "Null\n";
    }
};
