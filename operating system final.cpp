#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <cstdlib>
using namespace std;

// Task states
enum TaskState { RUNNING, WAITING, STOPPED, SUSPENDED };

string stateToString(TaskState state) {
    switch (state) {
        case RUNNING:   return "Running";
        case WAITING:   return "Waiting";
        case STOPPED:   return "Stopped";
        case SUSPENDED: return "Suspended";
        default:        return "Unknown";
    }
}

TaskState stringToState(const string& s) {
    if (s == "Running") return RUNNING;
    if (s == "Waiting") return WAITING;
    if (s == "Stopped") return STOPPED;
    if (s == "Suspended") return SUSPENDED;
    return RUNNING;
}

// Task class
class Task {
    int id;
    string name;
    int priority;
    int cpuUsage;    // Simulated CPU usage %
    int memoryUsage; // Simulated memory usage in MB
    TaskState state;
public:
    Task() {}
    Task(int _id, string _name, int _priority, int _cpu, int _mem, TaskState _state = RUNNING)
        : id(_id), name(_name), priority(_priority), cpuUsage(_cpu), memoryUsage(_mem), state(_state) {}

    int getId() const { return id; }
    string getName() const { return name; }
    int getPriority() const { return priority; }
    int getCPU() const { return cpuUsage; }
    int getMemory() const { return memoryUsage; }
    TaskState getState() const { return state; }

    void setState(TaskState s) { state = s; }
    void setPriority(int p) { priority = p; }
    void setCPU(int c) { cpuUsage = c; }
    void setMemory(int m) { memoryUsage = m; }
    void setName(const string& n) { name = n; }

    void display() const {
        cout << left << setw(5) << id
             << setw(18) << name
             << setw(10) << priority
             << setw(10) << cpuUsage
             << setw(12) << memoryUsage
             << setw(12) << stateToString(state)
             << endl;
    }
    // For file I/O
    void writeToFile(ofstream &ofs) const {
        ofs << id << '\n' << name << '\n' << priority << '\n'
            << cpuUsage << '\n' << memoryUsage << '\n' << stateToString(state) << '\n';
    }
    void readFromFile(ifstream &ifs) {
        string line;
        getline(ifs, line); id = atoi(line.c_str());
        getline(ifs, name);
        getline(ifs, line); priority = atoi(line.c_str());
        getline(ifs, line); cpuUsage = atoi(line.c_str());
        getline(ifs, line); memoryUsage = atoi(line.c_str());
        getline(ifs, line); state = stringToState(line);
    }
};

// Sorting functors for C++98
struct PriorityCompare {
    bool operator()(const Task &a, const Task &b) const {
        return a.getPriority() > b.getPriority();
    }
};
struct CPUCompare {
    bool operator()(const Task &a, const Task &b) const {
        return a.getCPU() > b.getCPU();
    }
};
struct MemoryCompare {
    bool operator()(const Task &a, const Task &b) const {
        return a.getMemory() > b.getMemory();
    }
};

// TaskManager class
class TaskManager {
    vector<Task> tasks;
    int nextId;
public:
    TaskManager() : nextId(1) {}

    void addTask() {
        string name;
        int priority, cpu, mem;
        cout << "Enter task name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter priority (1-10): ";
        cin >> priority;
        cout << "Enter CPU usage (%): ";
        cin >> cpu;
        cout << "Enter memory usage (MB): ";
        cin >> mem;
        tasks.push_back(Task(nextId++, name, priority, cpu, mem));
        cout << "Task added.\n";
    }

    void displayTasks(int filterState = -1) {
        int count = 0;
        cout << left << setw(5) << "ID"
             << setw(18) << "Name"
             << setw(10) << "Priority"
             << setw(10) << "CPU(%)"
             << setw(12) << "Memory(MB)"
             << setw(12) << "State"
             << endl;
        cout << string(67, '-') << endl;
        for (int i = 0; i < tasks.size(); ++i) {
            if (filterState == -1 || tasks[i].getState() == filterState) {
                tasks[i].display();
                count++;
            }
        }
        if (count == 0) cout << "No tasks to display for this filter.\n";
    }

    void endTask() {
        int id;
        cout << "Enter Task ID to stop: ";
        cin >> id;
        for (int i = 0; i < tasks.size(); ++i) {
            if (tasks[i].getId() == id && tasks[i].getState() != STOPPED) {
                tasks[i].setState(STOPPED);
                cout << "Task " << id << " stopped.\n";
                return;
            }
        }
        cout << "Task not found or already stopped.\n";
    }

    void terminateTask() {
        int id;
        cout << "Enter Task ID to terminate/delete: ";
        cin >> id;
        for (int i = 0; i < tasks.size(); ++i) {
            if (tasks[i].getId() == id) {
                tasks.erase(tasks.begin() + i);
                cout << "Task " << id << " terminated and removed.\n";
                return;
            }
        }
        cout << "Task not found.\n";
    }

    void changePriority() {
        int id, newPriority;
        cout << "Enter Task ID: ";
        cin >> id;
        cout << "Enter new priority (1-10): ";
        cin >> newPriority;
        for (int i = 0; i < tasks.size(); ++i) {
            if (tasks[i].getId() == id) {
                tasks[i].setPriority(newPriority);
                cout << "Priority updated.\n";
                return;
            }
        }
        cout << "Task not found.\n";
    }

    void editTask() {
        int id;
        cout << "Enter Task ID to edit: ";
        cin >> id;
        for (int i = 0; i < tasks.size(); ++i) {
            if (tasks[i].getId() == id) {
                string name;
                int priority, cpu, mem;
                cout << "Enter new task name: ";
                cin.ignore();
                getline(cin, name);
                cout << "Enter new priority (1-10): ";
                cin >> priority;
                cout << "Enter new CPU usage (%): ";
                cin >> cpu;
                cout << "Enter new memory usage (MB): ";
                cin >> mem;
                tasks[i].setName(name);
                tasks[i].setPriority(priority);
                tasks[i].setCPU(cpu);
                tasks[i].setMemory(mem);
                cout << "Task updated.\n";
                return;
            }
        }
        cout << "Task not found.\n";
    }

    void suspendTask() {
        int id;
        cout << "Enter Task ID to suspend: ";
        cin >> id;
        for (int i = 0; i < tasks.size(); ++i) {
            if (tasks[i].getId() == id && tasks[i].getState() == RUNNING) {
                tasks[i].setState(SUSPENDED);
                cout << "Task " << id << " suspended.\n";
                return;
            }
        }
        cout << "Task not found or not running.\n";
    }

    void resumeTask() {
        int id;
        cout << "Enter Task ID to resume: ";
        cin >> id;
        for (int i = 0; i < tasks.size(); ++i) {
            if (tasks[i].getId() == id && tasks[i].getState() == SUSPENDED) {
                tasks[i].setState(RUNNING);
                cout << "Task " << id << " resumed.\n";
                return;
            }
        }
        cout << "Task not found or not suspended.\n";
    }

    void searchTask() {
        int choice;
        cout << "Search by: 1. ID  2. Name\nEnter choice: ";
        cin >> choice;
        if (choice == 1) {
            int id;
            cout << "Enter Task ID: ";
            cin >> id;
            for (int i = 0; i < tasks.size(); ++i) {
                if (tasks[i].getId() == id) {
                    tasks[i].display();
                    return;
                }
            }
            cout << "Task not found.\n";
        } else if (choice == 2) {
            string name;
            cout << "Enter Task Name: ";
            cin.ignore();
            getline(cin, name);
            bool found = false;
            for (int i = 0; i < tasks.size(); ++i) {
                if (tasks[i].getName() == name) {
                    tasks[i].display();
                    found = true;
                }
            }
            if (!found)
                cout << "Task not found.\n";
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void sortTasks() {
        int choice;
        cout << "Sort by: 1. Priority 2. CPU usage 3. Memory usage\nEnter choice: ";
        cin >> choice;
        if (choice == 1) {
            sort(tasks.begin(), tasks.end(), PriorityCompare());
            cout << "Sorted by priority (high to low).\n";
        } else if (choice == 2) {
            sort(tasks.begin(), tasks.end(), CPUCompare());
            cout << "Sorted by CPU usage (high to low).\n";
        } else if (choice == 3) {
            sort(tasks.begin(), tasks.end(), MemoryCompare());
            cout << "Sorted by memory usage (high to low).\n";
        } else {
            cout << "Invalid choice.\n";
        }
    }

    void simulateStateChange() {
        // Randomly change state of a task (simulate waiting/running)
        if (tasks.empty()) return;
        int idx = rand() % tasks.size();
        if (tasks[idx].getState() == RUNNING)
            tasks[idx].setState(WAITING);
        else if (tasks[idx].getState() == WAITING)
            tasks[idx].setState(RUNNING);
        cout << "Simulated state change for Task ID " << tasks[idx].getId() << ".\n";
    }

    void saveToFile() {
        string fname;
        cout << "Enter filename to save: ";
        cin >> fname;
        ofstream ofs(fname.c_str());
        if (!ofs) {
            cout << "Could not open file for writing.\n";
            return;
        }
        ofs << tasks.size() << '\n';
        for (int i = 0; i < tasks.size(); ++i)
            tasks[i].writeToFile(ofs);
        ofs.close();
        cout << "Tasks saved to file.\n";
    }

    void loadFromFile() {
        string fname;
        cout << "Enter filename to load: ";
        cin >> fname;
        ifstream ifs(fname.c_str());
        if (!ifs) {
            cout << "Could not open file for reading.\n";
            return;
        }
        string line;
        getline(ifs, line);
        int n = atoi(line.c_str());
        tasks.clear();
        for (int i = 0; i < n; ++i) {
            Task t;
            t.readFromFile(ifs);
            tasks.push_back(t);
        }
        // Update nextId to be greater than any loaded id
        int maxid = 0;
        for (int i = 0; i < tasks.size(); ++i)
            if (tasks[i].getId() > maxid) maxid = tasks[i].getId();
        nextId = maxid + 1;
        ifs.close();
        cout << "Tasks loaded from file.\n";
    }

    void showResourceSummary() {
        int totalCPU = 0, totalMem = 0, running = 0, waiting = 0, stopped = 0, suspended = 0;
        for (int i = 0; i < tasks.size(); ++i) {
            totalCPU += tasks[i].getCPU();
            totalMem += tasks[i].getMemory();
            if (tasks[i].getState() == RUNNING) running++;
            else if (tasks[i].getState() == WAITING) waiting++;
            else if (tasks[i].getState() == STOPPED) stopped++;
            else if (tasks[i].getState() == SUSPENDED) suspended++;
        }
        cout << "\n--- System Resource Summary ---\n";
        cout << "Total tasks: " << tasks.size() << endl;
        cout << "Running: " << running << ", Waiting: " << waiting
             << ", Stopped: " << stopped << ", Suspended: " << suspended << endl;
        cout << "Total CPU usage: " << totalCPU << "%\n";
        cout << "Total Memory usage: " << totalMem << " MB\n";
    }

    void clearStoppedTasks() {
        int before = tasks.size();
        for (int i = 0; i < tasks.size(); ) {
            if (tasks[i].getState() == STOPPED)
                tasks.erase(tasks.begin() + i);
            else
                ++i;
        }
        cout << (before - tasks.size()) << " stopped tasks removed.\n";
    }
};

void menu() {
    TaskManager manager;
    int choice;
    while (true) {
        cout << "\n--- SYSTEM TASK MANAGER ---\n";
        cout << "1. Add Task\n";
        cout << "2. Display All Tasks\n";
        cout << "3. Display Running Tasks\n";
        cout << "4. Display Waiting Tasks\n";
        cout << "5. Display Stopped Tasks\n";
        cout << "6. Display Suspended Tasks\n";
        cout << "7. End (Stop) Task\n";
        cout << "8. Terminate (Delete) Task\n";
        cout << "9. Change Task Priority\n";
        cout << "10. Edit Task Details\n";
        cout << "11. Suspend Task\n";
        cout << "12. Resume Task\n";
        cout << "13. Search Task\n";
        cout << "14. Sort Tasks\n";
        cout << "15. Simulate State Change\n";
        cout << "16. Show System Resource Summary\n";
        cout << "17. Save Tasks to File\n";
        cout << "18. Load Tasks from File\n";
        cout << "19. Clear All Stopped Tasks\n";
        cout << "20. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;
        switch (choice) {
            case 1: manager.addTask(); break;
            case 2: manager.displayTasks(); break;
            case 3: manager.displayTasks(RUNNING); break;
            case 4: manager.displayTasks(WAITING); break;
            case 5: manager.displayTasks(STOPPED); break;
            case 6: manager.displayTasks(SUSPENDED); break;
            case 7: manager.endTask(); break;
            case 8: manager.terminateTask(); break;
            case 9: manager.changePriority(); break;
            case 10: manager.editTask(); break;
            case 11: manager.suspendTask(); break;
            case 12: manager.resumeTask(); break;
            case 13: manager.searchTask(); break;
            case 14: manager.sortTasks(); break;
            case 15: manager.simulateStateChange(); break;
            case 16: manager.showResourceSummary(); break;
            case 17: manager.saveToFile(); break;
            case 18: manager.loadFromFile(); break;
            case 19: manager.clearStoppedTasks(); break;
            case 20: return;
            default: cout << "Invalid choice.\n";
        }
    }
}

int main() {
    menu();
    return 0;
}

