#include <iostream>
#include <queue>
#include <stack>
#include <cstdlib>

using namespace std;

struct Node { //стркутура узла дерева
    int data;
    Node* left;
    Node* right;

    Node(const int _data = 0)
    {
        data = _data;
        left = nullptr;
        right = nullptr;
    }
};

class BinarySearchTree {

private:
    Node* root;

public:
    BinarySearchTree() //конструтор пустого корня
    {
        root = nullptr;
    }

    BinarySearchTree(const int &data) //конструктор корня с пустыми листьями
    {
        root = new Node(data);
    }

    BinarySearchTree(Node* node) //конструтор из готового узла
    {
        root = node;
    }

    //конструтор для создания рандомного дерева, здесь bottom и top это диапозон значений 
    //nodes_count нужен для задания количества вершин дерева
    BinarySearchTree(const int& bottom, const int& top, const int& nodes_count) 
    {
        if (nodes_count <= 0) { //если у дерева 0 узлов или меньше, то создаем пустой корень
            root = nullptr;
            return;
        }

        root = new Node(bottom + rand() % top);

        for (int node = 1; node < nodes_count; node++)
            this->insert(bottom + rand() % top);
    }

    BinarySearchTree(const BinarySearchTree& other) : root(nullptr) {
        copy(other.root, root);
    }

    ~BinarySearchTree() {
        destroy(root);
    }

    BinarySearchTree& operator=(const BinarySearchTree& other) {
        if (this != &other) {
            destroy(root);
            copy(other.root, root);
        }
        return *this;
    }

    Node* find(const int& key);
    Node* findMin();
    Node* findMax();
    void insert(const int& data);
    void remove(const int& key);
    void levelOrder();
    void inOrder();


    void destroy(Node* node) {
        if (node == NULL)
            return;

        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    void copy(Node* source, Node*& dest) {
        if (source == NULL)
            return;

        dest = new Node(source->data);
        copy(source->left, dest->left);
        copy(source->right, dest->right);
    }
};

Node* BinarySearchTree::find(const int& key) {
    Node* curr = root;
    while (curr && curr->data != key)
    {
        if (key < curr->data)
            curr = curr->left;
        else
            curr = curr->right;
    }
    return curr;
}

Node* BinarySearchTree::findMin() {
    Node* min = root;
    while (min && min->left)
        min = min->left;
    return min;
}

Node* BinarySearchTree::findMax() {
    Node* max = root;
    while (max && max->right)
        max = max->right;
    return max;
}

void BinarySearchTree::insert(const int &data) //вставка одного узла в дерево
{
    Node* newNode = new Node(data);
    Node* curr = root;
    Node* parent = nullptr;

    //алгоритм добавления узла в дерево
    while (curr) //в цикле идем до листьев
    {
        parent = curr; //запоинаем лист после которого будем вставлять
        if (data < curr->data)
            curr = curr->left;
        else
            curr = curr->right;
    }

    if (parent == nullptr) //если parrent пуст => дерево было пусто, newNode станет корнем дерева
        root = newNode;
    else if (data < parent->data) //если data < чем pareent.data значит новыый узел добаляем влево
        parent->left = newNode;
    else //иначе вправо
        parent->right = newNode;
}

void BinarySearchTree::remove(const int& key) //удаление узла по ключу
{
    Node* curr = root;
    Node* parent = nullptr;

    while (curr && curr->data != key) //в цикле находим узел с нужным ключем (после цикла в curr будет лежать узел с ключем)
    {
        parent = curr; //запоминаем родителя узела с ключем
        if (key < curr->data)
            curr = curr->left;
        else
            curr = curr->right;
    }

    if (!curr) //если не нашли узел с ключем то выходим
        return;

    if (!curr->left && !curr->right) //если узел для удаления это лист
    {
        if (curr == root) //если это корень то указатель корня в nullptr
            root = nullptr;

        else if (curr == parent->left) //если у лист это левый потомок у узла, то указатель на левого потомка в nullptr
            parent->left = nullptr;

        else //иначе лист это правый потомок, указатель на правого потомка в nullptr
            parent->right = nullptr;

        delete curr; //освобождаем лист (удаляем)
    }
    else if (curr->left && !curr->right) //если у узла с ключем есть потомок слева
    {
        if (curr == root) //если это корень то указатель корня = потомку слева 
            root = curr->left;

        else if (curr == parent->left) //если потомок с ключем слева, то перетягиваем левый повыше (как будто его просто перепрыгнули)
            parent->left = curr->left;

        else
            parent->right = curr->left; //если потомк с ключем справа, то так же перетягиваем повыше (как будто его просто перепрыгнули)

        delete curr; //освобождаем узел (удялем)
    }
    else if (!curr->left && curr->right) //если у узла с ключем есть потомок справа
    {
        if (curr == root) //если это корень то указатель корня = потомку справа 
            root = curr->right;

        else if (curr == parent->left) //перетягиваем как слева * (перепрыгиваем удаляемый)
            parent->left = curr->right;

        else
            parent->right = curr->right;

        delete curr; //освобождаем узел (удаляем)
    }
    else //если есть потомок и справа и слева, то будем выбирать наименьшего справа, чтобы заменить удаляемый
    {
        Node* successor = curr->right;
        parent = curr;
        while (successor->left) //в цикле ищем наименьший удаляемый
        {
            parent = successor;
            successor = successor->left;
        }
        curr->data = successor->data; // заменяем ключ удаляемого на ключ замены

        if (successor == parent->left) //если у замены есть кто то справа или слева, то он встанет на место замены, если нет, то на место замены встанет nullptr 
            parent->left = successor->right;
        else
            parent->right = successor->right;
        delete successor;
    }
}

void BinarySearchTree::levelOrder() //обход в ширину
{
    if (root == nullptr) //если корень пуст, то выходим
        return;

    queue<Node*> nodesQueue; //иначе создаем объект класса очередь

    nodesQueue.push(root); //добавляем корень в очередь

    Node* curr;

    while (!nodesQueue.empty()) //если очередь не пуста
    {
        curr = nodesQueue.front(); //в curr кладем ссылку на первый узел в очереди 

        cout << curr->data << ' '; //выводим ключ узла

        if (curr->left) // проверяем есть ли потомки у curr слева или справа и если они есть то добавляем в очереть
            nodesQueue.push(curr->left);
        if (curr->right)
            nodesQueue.push(curr->right);

        nodesQueue.pop(); //и удаляем элемент из начала очереди 
    }
}

void BinarySearchTree::inOrder() {
    

    if (root == nullptr)
        return;

    Node* curr = root;
    stack<Node*> nodesStack;
    nodesStack.push(curr);
    curr = curr->left;
    while (curr || !nodesStack.empty())
    {
        while (curr)
        {
            nodesStack.push(curr);
            curr = curr->left;
        }
        if (!nodesStack.empty())
        {
            curr = nodesStack.top();
            cout << curr->data << ' ';
            nodesStack.pop();
            if (curr->right)
                curr = curr->right;
            else
                curr = nullptr;
        }
    }
}

BinarySearchTree random_tree(int nodes_count) {
   
    if (nodes_count <= 0)
        return BinarySearchTree(0);

    BinarySearchTree tree;
    for (int node = 0; node < nodes_count; node++)
        tree.insert(-50 + rand() % 100);
    return tree;
}

int main(int argc, char const* argv[])
{
    srand(time(NULL));

    BinarySearchTree tree(14);

    tree.insert(5);
    tree.insert(15);
    tree.insert(3);
    tree.insert(7);
    tree.insert(13);
    tree.insert(17);
    tree.insert(7);

    //tree.levelOrder();

     tree.remove(1);
     tree.remove(17); 
     tree.remove(13);
     tree.remove(7);
     tree.remove(3);
     tree.remove(15);
     tree.remove(5);
     tree.remove(14);
     tree.remove(1);

     //tree.levelOrder();        

    // BinarySearchTree tree(0, 100, 14);
    // tree.levelOrder();
    Node* min = tree.findMin();
    Node* max = tree.findMax();
    cout << endl << "min " << min->data << " max " << max->data << endl;

    tree.inOrder();

    return 0;
}
