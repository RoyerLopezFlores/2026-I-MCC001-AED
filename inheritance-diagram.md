# Diagrama de Herencia

```mermaid
classDiagram
    direction LR

    %% Base trait
    class BaseContainerTrait~T,Node~

    %% Base iterator
    class general_iterator~Container,IteratorBase~

    %% Nodes
    class LLNode~T~
    class DLLNode~T~
    LLNode~T~ <|-- DLLNode~T~

    %% Linked list traits
    class BaseLinkedListTrait~T~
    class AscendingLinkedListTrait~T~
    class DescendingLinkedListTrait~T~

    BaseContainerTrait~T,LLNode<T>~ <|-- BaseLinkedListTrait~T~
    BaseLinkedListTrait~T~ <|-- AscendingLinkedListTrait~T~
    BaseLinkedListTrait~T~ <|-- DescendingLinkedListTrait~T~

    %% Double linked list traits
    class BaseDoubleLinkedListTrait~T~
    class AscendingDoubleLinkedListTrait~T~
    class DescendingDoubleLinkedListTrait~T~

    BaseContainerTrait~T,DLLNode<T>~ <|-- BaseDoubleLinkedListTrait~T~
    BaseDoubleLinkedListTrait~T~ <|-- AscendingDoubleLinkedListTrait~T~
    BaseDoubleLinkedListTrait~T~ <|-- DescendingDoubleLinkedListTrait~T~

    %% Circular linked list traits
    class BaseCircularLinkedListTrait~T~
    class AscendingCircularLinkedListTrait~T~
    class DescendingCircularLinkedListTrait~T~

    BaseContainerTrait~T,LLNode<T>~ <|-- BaseCircularLinkedListTrait~T~
    BaseCircularLinkedListTrait~T~ <|-- AscendingCircularLinkedListTrait~T~
    BaseCircularLinkedListTrait~T~ <|-- DescendingCircularLinkedListTrait~T~

    %% Circular double linked list traits
    class BaseCircularDoubleLinkedListTrait~T~
    class AscendingCircularDoubleLinkedListTrait~T~
    class DescendingCircularDoubleLinkedListTrait~T~

    BaseContainerTrait~T,DLLNode<T>~ <|-- BaseCircularDoubleLinkedListTrait~T~
    BaseCircularDoubleLinkedListTrait~T~ <|-- AscendingCircularDoubleLinkedListTrait~T~
    BaseCircularDoubleLinkedListTrait~T~ <|-- DescendingCircularDoubleLinkedListTrait~T~

    %% List containers
    class LinkedList~Traits~
    class DoubleLinkedList~Traits~
    class CircularLinkedList~Traits~
    class CircularDoubleLinkedList~Traits~

    LinkedList~Traits~ <|-- DoubleLinkedList~Traits~
    LinkedList~Traits~ <|-- CircularLinkedList~Traits~
    DoubleLinkedList~Traits~ <|-- CircularDoubleLinkedList~Traits~

    %% Linked-list iterators
    class LinkedListForwardIterator~Container~
    class DoubleLinkedListBackwardIterator~Container~
    class CircularLinkedListForwardIterator~Container~
    class CircularDoubleLinkedListForwardIterator~Container~
    class CircularDoubleLinkedListBackwardIterator~Container~

    general_iterator~Container,LinkedListForwardIterator<Container>~ <|-- LinkedListForwardIterator~Container~
    general_iterator~Container,DoubleLinkedListBackwardIterator<Container>~ <|-- DoubleLinkedListBackwardIterator~Container~
    general_iterator~Container,CircularLinkedListForwardIterator<Container>~ <|-- CircularLinkedListForwardIterator~Container~
    general_iterator~Container,CircularDoubleLinkedListForwardIterator<Container>~ <|-- CircularDoubleLinkedListForwardIterator~Container~
    general_iterator~Container,CircularDoubleLinkedListBackwardIterator<Container>~ <|-- CircularDoubleLinkedListBackwardIterator~Container~

    %% Vector side
    class VectorNode~T~
    class VectorTraits~T~
    class Vector~Traits~
    class vector_forward_iterator~Container~
    class vector_backward_iterator~Container~

    BaseContainerTrait~T,VectorNode<T>~ <|-- VectorTraits~T~
    general_iterator~Container,vector_forward_iterator<Container>~ <|-- vector_forward_iterator~Container~
    general_iterator~Container,vector_backward_iterator<Container>~ <|-- vector_backward_iterator~Container~
```

## Nota

El diagrama representa relaciones de herencia por clase y struct en los headers del proyecto.
