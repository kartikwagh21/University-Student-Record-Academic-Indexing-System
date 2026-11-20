// University Student Record & Academic Indexing System
// Additional challenges: 1, 2, 3 and 5
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>
using namespace std;

struct Student {
    int id, semester;
    string name, department, city;
    float cgpa;
};

void show(const vector<Student>& v) {
    if (v.empty()) { cout << "No records found.\n"; return; }
    cout << "\nID\tName\t\tDepartment\tSem\tCGPA\tCity\n";
    for (auto s : v)
        cout << s.id << "\t" << s.name << "\t" << s.department << "\t\t"
             << s.semester << "\t" << s.cgpa << "\t" << s.city << '\n';
}

// AVL TREE
class AVL {
    struct N {
        Student s; N *l, *r; int h;
        N(Student x): s(x), l(nullptr), r(nullptr), h(1) {}
    };
    N* root = nullptr;
    int ht(N* n) { return n ? n->h : 0; }
    void upd(N* n) { n->h = 1 + max(ht(n->l), ht(n->r)); }
    N* right(N* y) {
        N* x=y->l; y->l=x->r; x->r=y; upd(y); upd(x); return x;
    }
    N* left(N* x) {
        N* y=x->r; x->r=y->l; y->l=x; upd(x); upd(y); return y;
    }
    N* fix(N* n) {
        if (!n) return n;
        upd(n);
        int b=ht(n->l)-ht(n->r);
        if (b>1) { if (ht(n->l->l)<ht(n->l->r)) n->l=left(n->l); return right(n); }
        if (b<-1) { if (ht(n->r->r)<ht(n->r->l)) n->r=right(n->r); return left(n); }
        return n;
    }
    N* put(N* n, Student s) {
        if (!n) return new N(s);
        if (s.id<n->s.id) n->l=put(n->l,s);
        else if (s.id>n->s.id) n->r=put(n->r,s);
        else return n;
        return fix(n);
    }
    N* minNode(N* n) { while(n->l) n=n->l; return n; }
    N* erase(N* n,int id) {
        if (!n) return nullptr;
        if(id<n->s.id) n->l=erase(n->l,id);
        else if(id>n->s.id) n->r=erase(n->r,id);
        else {
            if(!n->l || !n->r) { N* c=n->l?n->l:n->r; delete n; return c; }
            N* m=minNode(n->r); n->s=m->s; n->r=erase(n->r,m->s.id);
        }
        return fix(n);
    }
    void walk(N* n, vector<Student>& v) const {
        if(!n) return; walk(n->l,v); v.push_back(n->s); walk(n->r,v);
    }
    void clean(N* n) { if(n){clean(n->l);clean(n->r);delete n;} }
public:
    ~AVL(){ clean(root); }
    bool has(int id) const { N* n=root; while(n){if(id==n->s.id)return true;n=id<n->s.id?n->l:n->r;}return false; }
    bool insert(Student s){if(has(s.id))return false;root=put(root,s);return true;}
    bool del(int id){if(!has(id))return false;root=erase(root,id);return true;}
    const Student* find(int id) const {N* n=root;while(n){if(id==n->s.id)return &n->s;n=id<n->s.id?n->l:n->r;}return nullptr;}
    vector<Student> all() const {vector<Student> v;walk(root,v);return v;}
    vector<Student> range(int a,int b) const {auto v=all();vector<Student> r;for(auto s:v)if(s.id>=a&&s.id<=b)r.push_back(s);return r;}
};

// B+ TREE (order 4; up to 3 keys per node)
class BPlus {
    struct N {
        bool leaf; vector<int> k; vector<Student> rec; vector<N*> ch;
        N *parent=nullptr,*next=nullptr;
        N(bool x):leaf(x){}
    };
    N* root=nullptr;
    int first(N* n) const {while(n&&!n->leaf)n=n->ch[0];return n->k[0];}
    void keys(N* n) {
        if(!n||n->leaf)return;
        n->k.clear(); for(size_t i=1;i<n->ch.size();++i)n->k.push_back(first(n->ch[i]));
    }
    void refresh(N* n){while(n){keys(n);n=n->parent;}}
    N* leafFor(int id) const {
        N* n=root;
        while(n&&!n->leaf){size_t i=upper_bound(n->k.begin(),n->k.end(),id)-n->k.begin();n=n->ch[i];}
        return n;
    }
    void addRight(N* a,N* b) {
        N* p=a->parent;
        if(!p){root=new N(false);root->ch={a,b};a->parent=b->parent=root;keys(root);return;}
        auto it=std::find(p->ch.begin(),p->ch.end(),a);
        p->ch.insert(it+1,b);b->parent=p;keys(p);
        if(p->ch.size()>4) splitInternal(p); else refresh(p->parent);
    }
    void splitInternal(N* n) {
        N* r=new N(false); size_t m=n->ch.size()/2;
        r->ch.assign(n->ch.begin()+m,n->ch.end());n->ch.erase(n->ch.begin()+m,n->ch.end());
        for(auto c:r->ch)c->parent=r;
        keys(n);keys(r);addRight(n,r);
    }
    void splitLeaf(N* n) {
        N* r=new N(true);size_t m=2;
        r->k.assign(n->k.begin()+m,n->k.end());r->rec.assign(n->rec.begin()+m,n->rec.end());
        n->k.erase(n->k.begin()+m,n->k.end());n->rec.erase(n->rec.begin()+m,n->rec.end());
        r->next=n->next;n->next=r;addRight(n,r);
    }
    void clear(N* n){if(!n)return;for(auto c:n->ch)clear(c);delete n;}
public:
    ~BPlus(){clear(root);}
    bool has(int id) const {
        N* n=leafFor(id);if(!n)return false;
        return binary_search(n->k.begin(),n->k.end(),id);
    }
    const Student* find(int id) const {
        N* n=leafFor(id);if(!n)return nullptr;
        auto it=lower_bound(n->k.begin(),n->k.end(),id);
        return it==n->k.end()||*it!=id?nullptr:&n->rec[it-n->k.begin()];
    }
    bool insert(Student s) {
        if(has(s.id))return false;
        if(!root){root=new N(true);root->k={s.id};root->rec={s};return true;}
        N* n=leafFor(s.id);auto it=lower_bound(n->k.begin(),n->k.end(),s.id);
        size_t i=it-n->k.begin();n->k.insert(it,s.id);n->rec.insert(n->rec.begin()+i,s);
        if(n->k.size()>3)splitLeaf(n);else refresh(n->parent);
        return true;
    }
    // Simple deletion: rebuild the B+ tree from remaining records.
    bool del(int id) {
        if(!has(id))return false;
        vector<Student> v=all();clear(root);root=nullptr;
        for(auto s:v)if(s.id!=id)insert(s);
        return true;
    }
    vector<Student> all() const {
        vector<Student> v;if(!root)return v;
        N* n=root;while(!n->leaf)n=n->ch[0];
        while(n){v.insert(v.end(),n->rec.begin(),n->rec.end());n=n->next;}
        return v;
    }
    vector<Student> range(int a,int b) const {
        vector<Student> v;N* n=leafFor(a);
        while(n){for(auto s:n->rec){if(s.id>=a&&s.id<=b)v.push_back(s);if(s.id>b)return v;}n=n->next;}
        return v;
    }
};

// CHALLENGES 1, 3 AND 5
void topCGPA(const vector<Student>& v){vector<Student> r;for(auto s:v)if(s.cgpa>9.0)r.push_back(s);show(r);}
void department(const vector<Student>& v){
    string d;cout<<"Department: ";cin>>d;vector<Student> r;
    for(auto s:v)if(s.department==d)r.push_back(s);show(r);
}
void save(const vector<Student>& v){
    ofstream f("students.csv");f<<"ID,Name,Department,Semester,CGPA,City\n";
    for(auto s:v)f<<s.id<<','<<s.name<<','<<s.department<<','<<s.semester<<','<<s.cgpa<<','<<s.city<<'\n';
    cout<<"Saved to students.csv\n";
}
vector<Student> load(){
    ifstream f("students.csv");vector<Student> v;string line;
    getline(f,line);
    while(getline(f,line)){
        stringstream ss(line);Student s;string cg;
        string id,sem;
        if(getline(ss,id,',')&&getline(ss,s.name,',')&&getline(ss,s.department,',')&&
           getline(ss,sem,',')&&getline(ss,cg,',')&&getline(ss,s.city,',')){
            try{s.id=stoi(id);s.semester=stoi(sem);s.cgpa=stof(cg);v.push_back(s);}catch(...){}
        }
    }
    return v;
}

// PERFORMANCE COMPARISON
template<class T> long long insertionTime(const vector<Student>& v){
    T t;auto a=chrono::high_resolution_clock::now();for(auto s:v)t.insert(s);
    return chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now()-a).count();
}
template<class T> long long searchTime(const vector<Student>& v){
    T t;for(auto s:v)t.insert(s);auto a=chrono::high_resolution_clock::now();
    for(int i=0;i<10000&&!v.empty();++i)t.find(v[i%v.size()].id);
    return chrono::duration_cast<chrono::microseconds>(chrono::high_resolution_clock::now()-a).count();
}
void compare(const vector<Student>& v){
    cout<<"\nTime in microseconds (search repeated 10,000 times)\n";
    cout<<"Operation\tAVL\tB+ Tree\n";
    cout<<"Insertion\t"<<insertionTime<AVL>(v)<<"\t"<<insertionTime<BPlus>(v)<<'\n';
    cout<<"Searching\t"<<searchTime<AVL>(v)<<"\t"<<searchTime<BPlus>(v)<<'\n';
    cout<<"Note: B+ Tree deletion rebuilds the tree; deletion timing is not compared.\n";
}

// MAIN MENU
int main(){
    vector<Student> data=load();
    if(data.empty()) data={
        {1001,5,"Aditi Sharma","CSE","Not specified",8.7},
        {1002,5,"Rahul Patil","IT","Not specified",7.9},
        {1003,3,"Neha Desai","AIML","Not specified",9.1},
        {1004,7,"Arjun Mehta","CSE","Not specified",8.2},
        {1005,3,"Sneha Kulkarni","IT","Not specified",8.5},
        {1006,5,"Rohan Shah","AIML","Not specified",7.6},
        {1007,3,"Priya Nair","CSE","Not specified",9.3},
        {1008,7,"Karan Joshi","IT","Not specified",8.0},
        {1009,5,"Isha Gupta","AIML","Not specified",8.9},
        {1010,7,"Aditya Rao","CSE","Not specified",7.8}
    };

    AVL avl;BPlus bp;
    vector<Student> uniqueData;
    for (const auto& s : data) {
        if (avl.insert(s)) {
            bp.insert(s);
            uniqueData.push_back(s);
        }
    }
    data = uniqueData; // Challenge 2: duplicate IDs are ignored

    int mainChoice;
    do{
        cout<<"\n=== STUDENT RECORD SYSTEM ===\n1. AVL Tree\n2. B+ Tree\n3. Compare Trees\n4. Save CSV\n5. Exit\nChoice: ";
        if(!(cin>>mainChoice)){cin.clear();cin.ignore(10000,'\n');continue;}
        if(mainChoice==1||mainChoice==2){
            int c;bool useAVL=mainChoice==1;
            do{
                cout<<"\n1.Add  2.Search  3.Delete  4.Display  5.ID Range\n6.Top CGPA  7.Department  8.Back\nChoice: ";
                cin>>c;
                if(c==1){
                    Student s;cout<<"ID: ";cin>>s.id;
                    if(avl.has(s.id)){cout<<"Duplicate ID!\n";continue;} // Challenge 2
                    cout<<"Name: ";cin>>ws;getline(cin,s.name);
                    cout<<"Department: ";cin>>s.department;
                    cout<<"Semester: ";cin>>s.semester;
                    cout<<"CGPA: ";cin>>s.cgpa;
                    cout<<"City: ";cin>>ws;getline(cin,s.city);
                    avl.insert(s);bp.insert(s);data.push_back(s);
                }else if(c==2){
                    int id;cout<<"ID: ";cin>>id;
                    auto p=useAVL?avl.find(id):bp.find(id);
                    if(p)show({*p});else cout<<"Student not found.\n";
                }else if(c==3){
                    int id;cout<<"ID: ";cin>>id;
                    if(avl.del(id)){bp.del(id);data.erase(remove_if(data.begin(),data.end(),
                        [id](Student s){return s.id==id;}),data.end());cout<<"Deleted.\n";}
                    else cout<<"Student not found.\n";
                }else if(c==4)show(useAVL?avl.all():bp.all());
                else if(c==5){
                    int a,b;cout<<"From ID to ID: ";cin>>a>>b;if(a>b)swap(a,b);
                    show(useAVL?avl.range(a,b):bp.range(a,b));
                }else if(c==6)topCGPA(avl.all());
                else if(c==7)department(avl.all());
            }while(c!=8);
        }else if(mainChoice==3)compare(data);
        else if(mainChoice==4)save(avl.all());
        else if(mainChoice==5){save(avl.all());cout<<"Goodbye!\n";}
    }while(mainChoice!=5);
    return 0;
}
