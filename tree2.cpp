#include <iostream>
#include <list>
#include <queue>

using namespace std;

class Node {
private:
    int key;
    Node* Left;
    Node* Right;
    friend class Ftree;

public:
    Node(int k = 0, Node* l = nullptr, Node* r = nullptr) {
        key = k;
        Left = l;
        Right = r;
    }

    int getValueKey() {
        return key;
    }
};

class Ftree {

protected:
    Node* root;
public:
    Ftree() {
        root = nullptr;
    }
    Ftree(int len) {
        root = nullptr;
        for (int i = 0; i < len; i++) {
            int value = rand() % 101;
            this->add(value);
        }
    }
    Ftree(int len, int* arr) {
        root = nullptr;
        for (int i = 0; i < len; i++) {
            this->add(arr[i]);
        }
    }
    ~Ftree() {
        delTree(this->root);
    }

    void delTree(Node* node) {
        if (node) {
            delTree(node->Left);
            delTree(node->Right);
            delete node;
        }
    }
    Node* addNode(Node* in, Node* from) {
        if (!from) {
            return nullptr;
        }
        else {
            in = new Node(from->key);
            in->Left = addNode(in->Left, from->Left);
            in->Right = addNode(in->Right, from->Right);
            return in;
        }
    }
    Ftree& operator = (const Ftree& tree) {
        if (this == &tree) {
            return *this;
        }
        else {
            delTree(this->root);
            root = addNode(this->root, tree.root);
        }
        return *this;
    }

    Ftree(Ftree& ftree) {
        Copy(this->root, ftree.root);
    }

    void Copy(Node*& x, Node* y) {
        if (y) {
            x = new Node(y->key);
            Copy(x->Left, y->Left);
            Copy(x->Right, y->Right);
        }
        else {
            x = nullptr;
        }
    }

    void add(int value) {
        if (root == nullptr) {
            root = new Node(value);
            return;
        }
        Node* node = root;
        while (1) {

            if (node->key > value && node->Left == nullptr) {
                node->Left = new Node(value);
                return;
            }
            else if (node->key <= value && node->Right == nullptr) {
                node->Right = new Node(value);
                return;
            }
            if (node->key > value)
                node = node->Left;
            else
                node = node->Right;
        }
    }

    bool search(int value) 
    {

        Node* node = root;
        while (1) {
            if (node == nullptr) {
                return false;
            }
            if (node->key == value) {
                return true;
            }

            if (value < node->key) {
                node = node->Left;
            }
            else if (value > node->key) {
                node = node->Right;
            }

        }
    }

    Node* min() {
        if (root == nullptr) {
            return nullptr;
        }
        Node* node = root;
        while (node->Left != nullptr) {
            node = node->Left;
        }
        return node;
    }
    Node* max() {
        if (root == nullptr) {
            return nullptr;
        }
        Node* node = root;
        while (node->Right != nullptr) {
            node = node->Right;
        }
        return node;
    }

    void print() {
        printTree(1, this->root);
        cout << endl;
    }

    void printTree(int indent, Node* node) {
        if (node != nullptr) {
            printTree(indent + 5, node->Right);
            for (int i = 0; i < indent; i++) {
                cout << " ";
            }
            cout << node->key << endl;
            printTree(indent + 5, node->Left);
        }
    }

    void LKR(Node* node = nullptr) {
        if (node == nullptr) {
            node = root;
        }
        if (node->Left) {
            LKR(node->Left);
        }
        cout << node->key << " ";
        if (node->Right) {
            LKR(node->Right);
        }
    }

    void Delete(int k)
    {
        Node* curr = root;
        Node* parent = nullptr;

        while (curr && curr->key != k) //в цикле находим узел с нужным ключем (после цикла в curr будет лежать узел с ключем)
        {
            parent = curr; //запоминаем родителя узела с ключем
            if (k < curr->key)
                curr = curr->Left;
            else
                curr = curr->Right;
        }

        if (!curr) //если не нашли узел с ключем то выходим
            return;



        if (!curr->Left && !curr->Right) //если узел для удаления это лист
        {

            if (curr == root) //если это корень то указатель корня в nullptr
                root = nullptr;

            if (curr == parent->Left) //если у лист это левый потомок у узла, то указатель на левого потомка в nullptr
                parent->Left = nullptr;

            else //иначе лист это правый потомок, указатель на правого потомка в nullptr
                parent->Right = nullptr;

            delete curr; //освобождаем лист (удаляем)
        }
        else if (curr->Left && !curr->Right) //если у узла с ключем есть потомок слева
        {
            if (curr == root) //если это корень то указатель корня = потомку слева 
                root = curr->Left;

            else if (curr == parent->Left) //если потомок с ключем слева, то перетягиваем левый повыше (как будто его просто перепрыгнули)
                parent->Left = curr->Left;

            else
                parent->Right = curr->Left; //если потомк с ключем справа, то так же перетягиваем повыше (как будто его просто перепрыгнули)

            delete curr; //освобождаем узел (удялем)
        }
        else if (!curr->Left && curr->Right) //если у узла с ключем есть потомок справа
        {
            if (curr == root) //если это корень то указатель корня = потомку справа 
                root = curr->Right;

            else if (curr == parent->Left) //перетягиваем как слева * (перепрыгиваем удаляемый)
                parent->Left = curr->Right;

            else
                parent->Right = curr->Right;

            delete curr; //освобождаем узел (удаляем)
        }
        else //если есть потомок и справа и слева, то будем выбирать наименьшего справа, чтобы заменить удаляемый
        {
            Node* successor = curr->Right;
            parent = curr;
            while (successor->Left) //в цикле ищем наименьший удаляемый
            {
                parent = successor;
                successor = successor->Left;
            }
            curr->key = successor->key; // заменяем ключ удаляемого на ключ замены

            if (successor == parent->Left) //если у замены есть кто то справа или слева, то он встанет на место замены, если нет, то на место замены встанет nullptr 
                parent->Left = successor->Right;
            else
                parent->Right = successor->Right;
            delete successor;
        }
    }


    void BFSprint() {
        BFS(this->root);
        cout << endl;
    }

	void BFS(Node* root) //обход в ширину
	{
		if (root == nullptr) //если корень пуст, то выходим
			return;

		queue<Node*> nodesQueue; //иначе создаем объект класса очередь

		nodesQueue.push(root); //добавляем корень в очередь

		Node* curr;

		while (!nodesQueue.empty()) //если очередь не пуста
		{
			curr = nodesQueue.front(); //в curr кладем ссылку на первый узел в очереди 

			cout << curr->key << ' '; //выводим ключ узла

			if (curr->Left) // проверяем есть ли потомки у curr слева или справа и если они есть то добавляем в очереть
				nodesQueue.push(curr->Left);
			if (curr->Right)
				nodesQueue.push(curr->Right);

			nodesQueue.pop(); //и удаляем элемент из начала очереди 
		}
	}

};




int main() {
    //srand(time(NULL));


    cout << "Case random tree 10 elements" << endl;
    Ftree tree(10);
    tree.print();
    cout << "_______________________________" << endl;
    
    tree.add(100);
    tree.print();


    cout << "Case Search " << endl;
    if (tree.search(41))
        cout << "Found " << endl;
    if (tree.search(80))
        cout << "Found " << endl;
    if (tree.search(68))
        cout << "Found " << endl;
    

    Node* min = tree.min();
    Node* max = tree.max();
    cout << "Case Min and Max: " << endl << min->getValueKey() << " " << max->getValueKey() << endl;

    cout << endl << endl;
    
    cout << "Case LKR: ";
    tree.LKR(); cout << endl;
    
    cout << "Case BFS: ";
    tree.BFSprint();
    cout << endl;

    
    cout << "Case Copy" << endl;
    Ftree treeCopy;
    treeCopy = tree;
    treeCopy.print();
    
    cout << "Case Delete" << endl;
    treeCopy.Delete(41);
    treeCopy.print();
    cout << "_______________________________" << endl;
     
    Ftree tree1(tree);
    tree1.print();
    return 0;
}


