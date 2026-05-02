#include "linkedlist.h"
#include <vector>
#include <thread>
#include <iostream>
#include <fstream>
using namespace std;

using DLI = LinkedList<DescendingLinkedListTrait<TI>>;

void test_push_front_concurrent() {
    DLI list;

    const int threads = 8;
    const int ops = 10000;

    vector<thread> ths;

    for (int t = 0; t < threads; ++t) {
        ths.emplace_back([&]() {
            for (int i = 0; i < ops; ++i) {
                list.push_front(i,i);
            }
        });
    }

    for (auto& t : ths) t.join();

    std::cout << "Push_Front :Size: " << list.size() << std::endl;
}
void test_push_back_concurrent() {
    DLI list;

    const int threads = 8;
    const int ops = 10000;

    vector<thread> ths;

    for (int t = 0; t < threads; ++t) {
        ths.emplace_back([&]() {
            for (int i = 0; i < ops; ++i) {
                list.push_back(i,i);
            }
        });
    }

    for (auto& t : ths) t.join();
    cout << "Push_Back :Size: " << list.size() << endl;
}
void test_push_pop_mixed() {
    DLI list;

    const int threads = 8;
    const int ops = 5000;

    vector<thread> ths;

    // productores
    for (int t = 0; t < threads / 2; ++t) {
        ths.emplace_back([&]() {
            for (int i = 0; i < ops; ++i) {
                list.push_front(i,i);
            }
        });
    }

    for (int t = 0; t < threads / 2; ++t) {
        ths.emplace_back([&]() {
            for (int i = 0; i < ops; ++i) {
                
                list.pop_front(); 
                
                
            }
        });
    }

    for (auto& t : ths) t.join();

    std::cout << "Push_Pop :Final size: " << list.size() << std::endl;
}
void test_copy_constructor() {
    DLI list;

    for (int i = 0; i < 10000; ++i)
        list.push_back(i,i);

    std::thread t1([&]() {
        DLI copy(list);
        cout << "Copy constructor: Check size" << (copy.size() == list.size()) << endl;
    });

    std::thread t2([&]() {
        for (int i = 0; i < 5000; ++i)
            list.push_back(i,i);
    });

    t1.join();
    t2.join();
}
void test_move_constructor() {
    DLI list;

    for (int i = 0; i < 10000; ++i)
        list.push_back(i,i);

    thread t1([&]() {
        DLI moved(std::move(list));
        std::cout << "Moved size: " << moved.size() << std::endl;
    });

    thread t2([&]() {
        for (int i = 0; i < 1000; ++i)
            list.push_back(i,i); // potencial conflicto
    });

    t1.join();
    t2.join();
}

//void test_destructor() {
//    auto list = new DLI();
//
//    thread t1([&]() {
//        for (int i = 0; i < 10000; ++i)
//            list->push_back(i,i);
//    });
//
//    thread t2([&]() {
//        delete list; // 💀 peligro si no está bien hecho
//    });
//
//    t1.join();
//    t2.join();
//    cout<<"Destructo test completed"<<endl;
//}
void stress_test() {
    DLI list;

    const int threads = 16;
    const int ops = 20000;

    vector<thread> ths;

    for (int t = 0; t < threads; ++t) {
        ths.emplace_back([&]() {
            for (int i = 0; i < ops; ++i) {
                if (i % 2)
                    list.push_front(i,i);
                else
                    
                    list.pop_back();
                    
            }
        });
    }

    for (auto& t : ths) t.join();

    std::cout << "Stress:  Final size: " << list.size() << std::endl;
}


template <typename T, typename Node>
bool IsGreaterThan(Node &node, T x){
    return node.getData() > x;
}
template<typename T, typename Node>
void AddX(Node &node, T x){
    node.getDataRef() += x;
}

void LinkedListDemo(){
    // 
    DLI list1;
    list1.insert(6, 15);
    list1.insert(2, 25);
    list1.insert(9, 35);
    list1.insert(1, 45);
    list1.insert(7, 55);
    cout << "Lista descendente: " << list1 << endl;

    LinkedList<AscendingLinkedListTrait<TI>> list2;
    list2.insert(6, 15);
    list2.insert(2, 25);
    list2.insert(9, 35);
    list2.insert(1, 45);
    list2.insert(7, 55);
    cout << "Lista ascendente : " << list2 << endl;

    ifstream ifs("entry.txt");
    DLI list3;
    
    ifs >> list3;
    
    cout<<"Lista desde archivo: " << list3 << endl;

    cout << "Recorrido con indices"<< endl;
    for( size_t i = 0 ; i < list1.size() ; ++i )
        cout << i<< ": " << list1[i] << ", ";
    cout << endl; 
    cout << "Push front test"<< endl;
    test_push_front_concurrent();
    cout << "Push back test"<< endl;
    test_push_back_concurrent();
    cout << "Push-Pop mixed test"<< endl;
    test_push_pop_mixed();
    cout << "Copy constructor test"<< endl;
    test_copy_constructor();
    cout << "Move constructor test"<< endl;
    test_move_constructor();
    //cout << "Destructor test"<< endl;
    //test_destructor();
    cout << "Stress test"<< endl;
    stress_test();

    cout<<"Recorrido con iteradores"<< endl;
    list3.ForEach([](DLI::Node &node){ cout << node.getData() << ":" << node.getRef() << ", "; });
    cout << endl;
    list3.ForEach(AddX<int, DLI::Node>, 10);
    cout << "Despues de agregar 10 a cada elemento: ";
    cout<<list3 << endl;
    cout<<endl;
    cout<< "Greater than 5: " << endl;
    auto it = list3.FirstThat(IsGreaterThan<int, DLI::Node>, 5);
    if (it != list3.end()) {
        cout << it->getData() << ":" << it->getRef() << endl;
    } else {
        cout << "No se encontro elemento" << endl;
    }
    ofstream ofs("output.txt");
    ofs << list3;
    cout<<"Lista guardada en output.txt" << endl;
}

void ListsDemo(){
    LinkedListDemo();
    
}
