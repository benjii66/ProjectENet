// ProjectENet.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "ENet.h"
#include <conio.h>

int getch_noblock() {
    if (_kbhit())
        return _getch();
    else
        return -1;
}


int main_simple2()
{
    ENet* networkLayer = new ENet();

    if (!networkLayer->Initialize())
        return EXIT_FAILURE;

    std::cout << "Hello Enet!\n\n";
    std::cout << "press 1 for Client\n";
    std::cout << "press 2 for Server\n";

    char choice;
    std::cin >> choice;

    switch (choice)
    {
    case '1':
        networkLayer->SetupClient();
        networkLayer->ConnectClient();
        break;

    case '2':
        networkLayer->SetupServer();
        break;

    default:
        std::cout << "unknown choice !\n";
        break;

    }



    std::cout << "ESC key to exit\n";

    bool bContinue = true;
    do
    {
        networkLayer->Update();

        // UpdateInput
        int lastKey = getch_noblock();
        if (lastKey < 0)
            continue;
        lastKey = toupper(lastKey);

        switch (lastKey)
        {
        case VK_ESCAPE:
            bContinue = false;
            break;
        }

    } while (bContinue);




    delete networkLayer;

    return 0;
}

int main()
{
    std::cout << "Hello World!\n";


    ENet* network = new ENet();
    if (!network->Initialize())
    {
        //error handling
    }
    else
    {
        network->SetupServer();
    }
    delete network;
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
