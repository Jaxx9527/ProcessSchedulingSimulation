#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <algorithm>
using namespace std;

struct PCB {// 进程控制块结构体
    string name;  // 进程名称
    int arrive_time;   // 到达时间
    int burst_time;  // 要求运行时间
    int remaining_time;   //剩余运行时间
    PCB* next; // 队列指针，指向下一个PCB（仅用于RR调度）
};

void sort_by_arrival(PCB* pcs, int N) {
    // 将阵列按到达时间排序
    for (int i = 0; i < N - 1; ++i) {
        int min_idx = i;
        for (int j = i + 1; j < N; ++j) {
            if (pcs[j].arrive_time < pcs[min_idx].arrive_time)
                min_idx = j;
        }
        if (min_idx != i){
            swap(pcs[i], pcs[min_idx]);//交换位置
        }
    }
}

void enqueue(PCB*& head, PCB*& tail, PCB* p) {
    // 就绪队列入队操作（RR）
    p->next = nullptr; // 新节点指向空
    if (!head) head = p; // 判断队列为空，如是，设置传进来的节点为头节点
    else tail->next = p; //否则将传进来的节点链接到队尾
    tail = p; // 更新队尾指针
}

PCB* dequeue(PCB*& head, PCB*& tail) {
    // 就绪队列出队操作（RR）
    if (!head) return nullptr;  //判断队列是否为空，如空返回nullptr
    PCB* p = head;//取出队首
    head = head->next;//更新队首
    if (!head) tail = nullptr; // 队列空则更新队尾
    p->next = nullptr; // 断开链接
    return p;
}


void dd_rr(PCB* pcs, int N) {
    // 抢占式 Round Robin 调度实现
    PCB* head = nullptr;
    PCB* tail = nullptr;
    int time = 0, completed = 0, next_idx = 0; // 初始化变量：当前时间、完成的进程数、下一个未入队进程的索引
    //start 初始时将所有已到达进程入队
    while (next_idx < N && pcs[next_idx].arrive_time <= time) {
        enqueue(head, tail, &pcs[next_idx]);
        ++next_idx;
    }
    //end 初始时将所有已到达进程入队

    cout << "\n使用 Round Robin 调度算法\n";
    while (completed < N) {    // 循环直到所有进程完成
        if (!head) { // 判断队列是否为空，如是CPU空闲
            cout << "Time " << time << ": CPU 空闲\n\n";
            ++time;
            // 检查新到达进程
            while (next_idx < N && pcs[next_idx].arrive_time <= time) {
                enqueue(head, tail, &pcs[next_idx]);
                ++next_idx;
            }
            continue;
        }
        // 出队一个进程执行一个时间片
        PCB* cur = dequeue(head, tail);
        cout << "Time " << time << ": 运行进程名 " << cur->name;
        --(cur->remaining_time);// 剩余时间减 1
        ++time; //时间前进
        cout << " (剩余 " << cur->remaining_time +1<< "->"<<cur->remaining_time << ")" ;
        // 检查当前时间点的新到达进程
        while (next_idx < N && pcs[next_idx].arrive_time <= time) {
            enqueue(head, tail, &pcs[next_idx]);
            ++next_idx;
        }
        // 若当前进程尚未完成，重新入队
        if (cur->remaining_time > 0) enqueue(head, tail, cur);
        else {
            // 进程完成，从队列中移除
            cout << "\t<进程 " << cur->name << " 已完成，踢出队列>" ;
            ++completed;
        }
        if (!head) {
            cout << "\n\t\t队列: 空\n";
        }else {
            cout << "\n\t\t队列: ";
            for (PCB* p = head; p; p = p->next) {
                cout << p->name << "(剩余 " << p->remaining_time << ")\t";
            }
        }
        cout << endl<<endl;
    }
    cout << "所有进程调度完成，总时间 = " << time << endl;
}

void dd_sjf(PCB* pcs, int N) {
    // 抢占式SJF调度实现
    vector<PCB*> ready; // 初始化就绪队列变量
    int time = 0, completed = 0, next_idx = 0;//初始化变量：系统当前时间、已完成进程计数、下一个待入队进程索引
    cout << "\n使用 抢占式短作业优先 调度算法\n";


    while (completed < N) { //主循环：直到所有进程完成

        // start 把到达的进程加入 ready
        while (next_idx < N && pcs[next_idx].arrive_time <= time) {
            ready.push_back(&pcs[next_idx]);
            ++next_idx;
        }
        // end 把到达的进程加入 ready

        if (ready.empty()) {// 判断ready是否为空，如是则CPU空闲
            cout << "Time " << time << ": CPU 空闲\n\n";
            ++time;
            continue;
        }
        // 选剩余时间最短
        auto cmp = [](PCB* a, PCB* b) { return a->remaining_time < b->remaining_time; };// 定义比较函数：返回true表示a 的剩余时间小于b
        auto it = min_element(ready.begin(), ready.end(), cmp);// 找到ready中最短进程的迭代器
        PCB* cur = *it; //解引用迭代器得到当前选中的进程
        cout << "Time " << time << ": 运行进程名 " << cur->name;//打印当前时间和进程名称，表示开始执行
        --(cur->remaining_time); // 执行一个时间单位，进程剩余时间减1
        ++time;// 时间+1
        cout << " (剩余 " << cur->remaining_time +1<< "->"<<cur->remaining_time << ")";
        // 完成检测
        if (cur->remaining_time == 0) {// 判断进程是否完成，如是，移除并计数
            cout << "\t<进程 " << cur->name << " 已完成，踢出队列>" ;
            ready.erase(it);
            ++completed;
        }

        if (ready.empty()) cout << "\n\t\t队列: 空" << endl;
        else {
            cout << "\n\t\t队列: ";
            for (auto p : ready) cout << p->name << "(" << p->remaining_time << ") ";
        }

        cout <<endl<< endl;
    }
    cout << "所有进程调度完成，总时间 = " << time << endl;
}

int main() {
    int N,dd_choice;
    cout << "\n选择调度算法 (1: Round Robin; 2: 抢占式 SJF):  ";
    cin >> dd_choice;
    cout << "请输入进程数量 (N>=4): ";
    while (!(cin >> N) || N < 4) {
        cout << "输入无效，请输入 N>=4 的整数: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    PCB* pcs = new PCB[N];    // 动态分配PCB阵列
    for (int i = 0; i < N; ++i) {
        cout << "\n进程 " << i+1 << " 名称: "; cin >> pcs[i].name;
        cout << "进程 " << i+1 << " 到达时间: ";    cin >> pcs[i].arrive_time;
        cout << "进程 " << i+1 << " 要求运行时间: "; cin >> pcs[i].burst_time;
        pcs[i].remaining_time = pcs[i].burst_time;
        pcs[i].next = nullptr;
    }
    sort_by_arrival(pcs, N); //按到达时间排序

    if (dd_choice == 1) dd_rr(pcs, N);
    else if (dd_choice == 2) dd_sjf(pcs, N);
    else cout << "无效选择，程序结束" << endl;
    delete[] pcs;//  释放内存
    return 0;
}
