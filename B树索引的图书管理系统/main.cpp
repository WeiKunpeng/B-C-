#include<iostream>
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<conio.h>

using namespace std;
#define N 10000
#define m 3   //B树的阶

struct book       //书的结构体
{
    unsigned int key;    //书号
    char name[20];     //书名
    char author[20];    //作者
    unsigned int stock;       //现存量
    unsigned int total;    //总库存量
} b[N];    //存储书的数组

typedef struct BTreeNode
{
    int keynum;    //结点关键值/关键码个数
    struct BTreeNode *parent;   //父结点指针
    book key[m+1];     //一个结点中最多存放的书的个数，k[0]未用
    struct BTreeNode *ptr[m+1];   //子树结点数组指针
}BTNode;
BTNode *root=NULL;

typedef struct     //搜素结果的三元组定义
{
    BTNode *pt;      //结点地址指针
    int i;           //结点中关键码序号
    int tag;       //tag=0不成功，tag=1成功
}Result;

int Search(BTNode *p,book k)
//在一个结点中查找元素，返回结点位置
{
    if(!p)
        return -1;
    int i=0;
    for(i=0; i<p->keynum&&p->key[i+1].key<=k.key; i++);
    return i;
}

Result SearchBTree(BTNode *T,book k)
//在m阶B树上查找关键字K，返回结果（pt,i,tag）
{
    BTNode *p,*q;
    int i;
    Result R;
    p=T;
    q=NULL;
    int found=0;
    i=0;
    while(p&&!found)
    {
        i=Search(p,k);
        if(i>0&&p->key[i].key==k.key)
            found=1;
        else
        {
            q=p;
            p=p->ptr[i];
        }
    }
    if(found)
    {
        R.pt=p;
        R.i=i;
        R.tag=1;   //找到，成功
    }
    else
    {
        R.pt=q;
        R.i=i;
        R.tag=0;    //不成功
    }
    return R;
}

void Copy(book &b,book k)
//复制一个结点
{
    b.key=k.key;
    strcpy(b.name,k.name);
    strcpy(b.author,k.author);
    b.stock=k.stock;
    b.total=k.total;
}

void Insert(BTNode* &q,int i,book x,BTNode *ap)
//插入一条记录
{
    int n=q->keynum;
    for(int j=n;j>i;j--)
    {
        Copy(q->key[j+1],q->key[j]);
        q->ptr[j+1]=q->ptr[j];
    }
    Copy(q->key[i+1],x);
    q->ptr[i+1]=ap;
    if(ap)
        ap->parent=q;
    q->keynum++;
}

void split(BTNode* &q,int s,BTNode *&ap)
//分裂结点
{
    int i,j,n=q->keynum;
    ap=(BTNode*)malloc(sizeof(BTNode));
    ap->ptr[0]=q->ptr[s];
    for(i=s+1,j=1;i<=n;i++,j++)
    {
        Copy(ap->key[j],q->key[i]);
        ap->ptr[j]=q->ptr[i];
    }
    ap->keynum=n-s;
    ap->parent=q->parent;
    for(i=0;i<=n-s;i++)
    {
        if(ap->ptr[i])
            ap->ptr[i]->parent=ap;
    }
    q->keynum=s-1;
}

void NewRoot(BTNode* &T,BTNode* p,book x,BTNode *ap)
//生成一个新的树根结点
{
    T=(BTNode*)malloc(sizeof(BTNode));
    T->keynum=1;      //设置当前结点的元素个数
    T->ptr[0]=p;      //设置左边结点的树根
    T->ptr[1]=ap;     //设置右边的树根
    Copy(T->key[1],x);   //将x元素的结点值复制到T的第一个元素中
    if(p)             //当孩子不空时就设置当前结点为孩子的双亲
        p->parent=T;
    if(ap)
        ap->parent=T;
    T->parent=NULL;   //当前结点双亲为空
}

void ShowBTNode(BTNode *p)
//输出书的信息
{
    for(int i=1;i<=p->keynum;i++)
    {
        cout<<"书号"<<p->key[i].key<<",书名为《"<<p->key[i].name<<"》，作者为"<<p->key[i].author<<",现存"<<p->key[i].stock<<"本，总量"<<p->key[i].total<<"本。"<<endl;
    }
}

void ShowBookMess(book bk)
//显示书的信息，参数是book类型
{
    cout<<"书号"<<bk.key<<",书名为《"<<bk.name<<"》，作者为"<<bk.author<<",现存"<<bk.stock<<"本，总量"<<bk.total<<"本。"<<endl;
}

int InsertBTNode(BTNode *&T,book k)
//插入新书，在B树中插入新结点
//在m阶B树T上结点*q的key[i]与k[i+1]之间插入关键字k;
{
    BTNode *ap;
    Result r;
    BTNode *q;
    int i;
    char addnum;
    int finished,needNewRoot,s;
    book x;
    if(!T)   //root为NULL时，将book存储在根结点
    {
        NewRoot(T,NULL,k,NULL);
    }
    else
    {
        r=SearchBTree(T,k);  //查找元素k在树中的位置
        q=r.pt;
        i=r.i;
        if(r.tag==1)    //有相同的书号
        {
            if(strcmp(q->key[i].name,k.name)!=0)   //当书名不等时
            {
                cout<<"已存在书号为"<<q->key[i].key<<"的书，录入失败，请重新编号！"<<endl;
                ShowBookMess(q->key[i]);
                return 0;
            }
            else
            {
                cout<<"该书已存在，是否增加其总存储量（y/n）:";
                cin>>addnum;
                if(addnum=='Y'||addnum=='y')
                {
                    q->key[i].total+=k.total;
                    q->key[i].stock+=k.stock;
                    cout<<"增加总量后书的信息如下:"<<endl;
                }
                else
                {
                    cout<<"该书的信息如下："<<endl;
                }
                ShowBookMess(q->key[i]);
                return 0;
            }
        }
        x=k;           //没有相同的书号时，插入
        ap=NULL;
        finished=needNewRoot=0;
        while(!needNewRoot&&!finished)
        {
            Insert(q,i,x,ap);  //插入结点
            if(q->keynum<m)
                finished=1;   //插入完成
            else
            {     //分裂结点*q
                s=(m+1)/2;
                split(q,s,ap);
                x=q->key[s];
                if(q->parent)   //在双亲结点*q中查找x的插入位置
                {
                    q=q->parent;
                    i=Search(q,x);
                }
                else
                {
                     needNewRoot=1;
                }

            }
        }
        if(needNewRoot)
            NewRoot(T,q,x,ap);   //生成新根结点*T,q和ap为子树指针
    }
    return 1;
}

void InBookMess(book &bk)
//输入书的信息
{
    cout<<"请输入书号：";cin>>bk.key;
    cout<<"请输入书名：";cin>>bk.name;
    cout<<"请输入作者：";cin>>bk.author;
    cout<<"请输入总量：";cin>>bk.total;
    bk.stock=bk.total;
}

void display(BTNode *T)
//显示整棵树的信息
{
    int i=0;
    if(T)
    {
        ShowBTNode(T);   //显示该结点的全部值
        for(i=0;i<=T->keynum;i++)    //递归显示每个结点
        {
            if(T->ptr[i])
                display(T->ptr[i]);
        }
    }
}

int BorrowBook(BTNode *T,book k)
//借书
{
    Result r=SearchBTree(T,k);
    if(r.tag==0)
    {
        cout<<"你要借阅的书不存在。"<<endl;
        return 0;
    }
    if(r.pt->key[r.i].stock<1)
    {
        cout<<"你要借阅的书已经借完。"<<endl;
        return 0;
    }
    r.pt->key[r.i].stock--;
    return 1;
}

int ReturnBook(BTNode *T,book k)
//还书
{
    //int number;
    Result r=SearchBTree(T,k);
    if(r.tag==0)
    {
        cout<<"抱歉！不存在你要还的书。"<<endl;
        return 0;
    }
    if(r.pt->key[r.i].stock>=r.pt->key[r.i].total)
    {
        cout<<"该书无借出。"<<endl;
        return 0;
    }
    else
        r.pt->key[r.i].stock++;
}

void save(BTNode *p)
//保存文件
{
    FILE *fp;
    if((fp=fopen("book.txt","wb"))==NULL)
    {
        cout<<"创建文件失败！"<<endl;
        getch();
        return;
    }
    for(int i=1;i<=p->keynum;i++)
    {
        fprintf(fp,"%d %s %s %d %d \n",p->key[i].key,p->key[i].name,p->key[i].author,p->key[i].stock,p->key[i].total);
    }
    fclose(fp);
}

void read()
//读取文件
{
    FILE *fp,fp1;
    if((fp=fopen("book.txt","rb"))==NULL)
    {
        cout<<"读取文件失败！"<<endl;
        getchar();
        return;
    }
    for(int i=1;;i++)
    {
        if(fscanf(fp,"%d%s%s%d%d",&b[i].key,&b[i].name,&b[i].author,&b[i].stock,&b[i].total)==EOF)
        {
            break;
        }
        InsertBTNode(root,b[i]);
    }
    fclose(fp);
}

void output(BTNode* T,int n)   //凹入法输出这课树
{
    int i;
    if(T==NULL)
        return;
/*    if(n!=1)
    {
        for(i=1;i<n;i++)
          cout<<"----";
    }*/
    for(int j=1;j<=T->keynum;j++)
    {
        cout<<T->key[j].key<<endl;
        cout<<"----";
    }
    for(i=0;i<=T->keynum;i++)
        {
            if(T->ptr[i])
                output(T->ptr[i],n++);
        }
}

int menu_selete()
{
    int choose;
    system("cls");
    cout<<"**********图书管理系统************"<<endl;
    cout<<"1.新书入库\t 2.查找书籍"<<endl;
    cout<<"3.显示库存\t 4.借阅书籍"<<endl;
    cout<<"5.归还书籍\t 6.凹入法显示树"<<endl;
    cout<<"7.退出系统"<<endl;
    cout<<"***********************************"<<endl;
    cout<<"请输入你需要的操作（1~7）：";
    do
    {
        cin>>choose;
    }while(choose<1||choose>7);
    return choose;
}

int main()
{
    book k;
    Result r;
    read();
    while(1)
    {
        switch(menu_selete())
        {
            case 1:
                system("cls");
                cout<<"录入书信息："<<endl;
                InBookMess(k);
                InsertBTNode(root,k);
                cout<<"录入结束。"<<endl;
                save(root);
                cout<<"当前书库的库存信息如下:"<<endl;
                display(root);
                cout<<"按任意键返回";
                getch();
                break;
            case 2:
                system("cls");
                cout<<"查找书信息："<<endl;
                cout<<"请输入书号：";
                cin>>k.key;
                r=SearchBTree(root,k);
                if(r.tag==1)
                {
                    ShowBookMess(r.pt->key[r.i]);
                }
                else
                    cout<<"你要查找的书号"<<k.key<<"不存在！"<<endl;
                cout<<"查找结束"<<endl;
                cout<<"按任意键返回";
                getch();
                break;
            case 3:
                system("cls");
                cout<<"-----------全部的书------------"<<endl;
                display(root);
                cout<<"-----------显示完毕------------"<<endl;
                cout<<"按任意键返回";
                getch();
                break;
            case 4:
                system("cls");
                cout<<"借书："<<endl;
                cout<<"请输入书号：";
                cin>>k.key;
                if(BorrowBook(root,k))
                {
                    cout<<"借书成功。"<<endl;
                }
                else
                    cout<<"借书失败。"<<endl;
                cout<<"按任意键返回";
                getch();
                break;
            case 5:
                system("cls");
                cout<<"还书："<<endl;
                cout<<"请输入书号：";
                cin>>k.key;
                if(ReturnBook(root,k))
                {
                    cout<<"归还成功。"<<endl;
                }
                else
                    cout<<"归还失败。"<<endl;
                cout<<"按任意键返回";
                getch();
                break;
            case 6:
                system("cls");
                cout<<"凹入法显示树："<<endl;
                output(root,1);
                cout<<"按任意键返回";
                getch();
                break;
            case 7:
                save(root);
                exit(0);
        }

    }

}
