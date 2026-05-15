#include <iostream>
#include <fstream>
#include <string>
#include <stack>
using namespace std;
const int MAX = 50;
// ==================== DATA STRUCTURES ====================
struct Teacher {
    string name;
    string subject;
};

struct ClassData {
    string className;
    string subject;
};

struct Room {
    string roomName;
};

struct TimeSlot {
    string slot;
};

struct ClassSession {
    string className;
    string teacher;
    string room;
    string subject;
};

struct Edge {
    int u, v;
    int weight;
};

// ==================== GLOBAL VARIABLES ====================
int distanceMatrix[MAX][MAX];
Teacher teachers[MAX];
ClassData classesArr[MAX];
Room rooms[MAX];
TimeSlot timeSlots[MAX];
int graph[MAX][MAX];
int conflictGraph[MAX][MAX];
ClassSession sessions[MAX];
string sessionTime[MAX];
int sessionColor[MAX];
int eulerPath[MAX];
int hamiltonPath[MAX];
int parent[MAX];

int teacherCount = 0, classCount = 0;
int roomCount = 0, slotCount = 0;
int sessionCount = 0;
int eulerIndex = 0;

// ==================== CLASS: FILE MANAGER (MODULE 1) ====================
class FileManager {
public:
    void loadTeachers() {
        ifstream file("teachers.txt");
        if (!file) {
            cout << "Error: teachers.txt not found!\n";
            return;
        }
        string line;
        while (getline(file, line) && teacherCount < MAX) {
            if (line.empty()) continue;
            size_t pos = line.find(',');
            if (pos == string::npos) continue;
            teachers[teacherCount].name = line.substr(0, pos);
            teachers[teacherCount].subject = line.substr(pos + 1);
            teacherCount++;
        }
        file.close();
    }

    void loadClasses() {
        ifstream file("classes.txt");
        if (!file) {
            cout << "Error: classes.txt not found!\n";
            return;
        }
        string line;
        while (getline(file, line) && classCount < MAX) {
            if (line.empty()) continue;
            size_t pos = line.find(',');
            if (pos == string::npos) continue;
            classesArr[classCount].className = line.substr(0, pos);
            classesArr[classCount].subject = line.substr(pos + 1);
            classCount++;
        }
        file.close();
    }

    void loadRooms() {
        ifstream file("rooms.txt");
        if (!file) {
            cout << "Error: rooms.txt not found!\n";
            return;
        }
        string line;
        while (getline(file, line) && roomCount < MAX) {
            if (!line.empty()) {
                rooms[roomCount].roomName = line;
                roomCount++;
            }
        }
        file.close();
    }

    void loadTimeSlots() {
        ifstream file("timeslots.txt");
        if (!file) {
            cout << "Error: timeslots.txt not found!\n";
            return;
        }
        string line;
        while (getline(file, line) && slotCount < MAX) {
            if (!line.empty()) {
                timeSlots[slotCount].slot = line;
                slotCount++;
            }
        }
        file.close();
    }

    void loadDistanceMatrix() {
        ifstream file("distances.txt");
        if (!file) {
            cout << "Error: distances.txt not found!\n";
            return;
        }
        for (int i = 0; i < classCount && i < MAX; i++)
            for (int j = 0; j < classCount && j < MAX; j++)
                file >> distanceMatrix[i][j];
        file.close();
    }

    void initializeSystem() {
        cout << "\n--- Loading Smart Timetable Data ---\n";
        loadTeachers();
        loadClasses();
        loadRooms();
        loadTimeSlots();
        loadDistanceMatrix();
        cout << "\nData Loaded Successfully!\n";
        cout << teacherCount << " Teachers Loaded\n";
        cout << classCount << " Classes Loaded\n";
        cout << roomCount << " Rooms Loaded\n";
        cout << slotCount << " Time Slots Loaded\n\n";
    }
};

// ==================== CLASS: GRAPH MANAGER (MODULES 2, 3, 5, 6) ====================
class GraphManager {
public:
    // MODULE 2: Build Sessions
    void buildSessions() {
        sessionCount = classCount;
        if (teacherCount == 0 || roomCount == 0) {
            cout << "ERROR: Teachers or Rooms not loaded!\n";
            return;
        }
        for (int i = 0; i < classCount; i++) {
            sessions[i].className = classesArr[i].className;
            sessions[i].subject = classesArr[i].subject;
            sessions[i].teacher = "Unknown";
            // Assign teacher based on subject
            for (int j = 0; j < teacherCount; j++) {
                if (teachers[j].subject == sessions[i].subject) {
                    sessions[i].teacher = teachers[j].name;
                    break;
                }
            }
            sessions[i].room = rooms[i % roomCount].roomName;
        }
    }

    // MODULE 3: Logic Checking & Validation
    bool shareTeacher(int a, int b) {
        if (a >= sessionCount || b >= sessionCount) return false;
        if (sessions[a].teacher == "Unknown" || sessions[b].teacher == "Unknown")
            return false;
        return sessions[a].teacher == sessions[b].teacher;
    }

    bool shareRoom(int a, int b) {
        if (a >= sessionCount || b >= sessionCount) return false;
        return sessions[a].room == sessions[b].room;
    }

    bool sameTime(int a, int b) {
        if (a >= sessionCount || b >= sessionCount) return false;
        return sessionTime[a] == sessionTime[b] && !sessionTime[a].empty();
    }

    bool validateTeacherTimeRule(int a, int b) {
        return !(shareTeacher(a, b) && sameTime(a, b));
    }

    bool validateRoomTimeRule(int a, int b) {
        return !(shareRoom(a, b) && sameTime(a, b));
    }

    // Build conflict graph based on logical constraints
    void buildConflictGraph() {
        for (int i = 0; i < sessionCount; i++)
            for (int j = 0; j < sessionCount; j++)
                conflictGraph[i][j] = 0;

        for (int i = 0; i < sessionCount; i++) {
            for (int j = i + 1; j < sessionCount; j++) {
                if (shareTeacher(i, j) || shareRoom(i, j)) {
                    conflictGraph[i][j] = 1;
                    conflictGraph[j][i] = 1;
                }
            }
        }

        cout << "\n=== MODULE 3: Conflict Graph Built ===\n";
        int edgeCount = 0;
        for (int i = 0; i < sessionCount; i++)
            for (int j = i + 1; j < sessionCount; j++)
                if (conflictGraph[i][j] == 1)
                    edgeCount++;
        cout << "Total Sessions: " << sessionCount << "\n";
        cout << "Conflict Edges: " << edgeCount << "\n";
        cout << "(Sessions sharing teacher or room must have different time slots)\n\n";
    }

    // Generate validation report AFTER scheduling
    void generateValidationReport() {
        ofstream report("logic_validation.txt");
        report << "=== MODULE 3: Logic Checking & Validation ===\n\n";
        report << "Propositional Logic Rules:\n";
        report << "Rule 1: NOT(SameTeacher(c1, c2) AND SameTime(c1, c2))\n";
        report << "        [A teacher cannot be in two places at once]\n\n";
        report << "Rule 2: NOT(SameRoom(c1, c2) AND SameTime(c1, c2))\n";
        report << "        [A room cannot host two classes simultaneously]\n\n";

        bool allRulesPassed = true;
        int violationCount = 0;

        report << "Validation Results:\n";
        report << "-------------------\n";

        for (int i = 0; i < sessionCount; i++) {
            for (int j = i + 1; j < sessionCount; j++) {
                if (!validateTeacherTimeRule(i, j)) {
                    report << "VIOLATION: " << sessions[i].className
                        << " and " << sessions[j].className
                        << " share teacher (" << sessions[i].teacher
                        << ") at same time (" << sessionTime[i] << ")\n";
                    allRulesPassed = false;
                    violationCount++;
                }

                if (!validateRoomTimeRule(i, j)) {
                    report << "VIOLATION: " << sessions[i].className
                        << " and " << sessions[j].className
                        << " share room (" << sessions[i].room
                        << ") at same time (" << sessionTime[i] << ")\n";
                    allRulesPassed = false;
                    violationCount++;
                }
            }
        }

        if (allRulesPassed) {
            report << "\n[PASS] ALL LOGIC RULES SATISFIED\n";
            report << "No conflicts detected. Timetable is logically consistent.\n";
        }
        else {
            report << "\n[FAIL] LOGIC VIOLATIONS FOUND: " << violationCount << "\n";
            report << "Timetable has conflicts that violate scheduling constraints.\n";
        }

        report << "\nConflict Graph Summary:\n";
        report << "Total Sessions: " << sessionCount << "\n";
        int edgeCount = 0;
        for (int i = 0; i < sessionCount; i++)
            for (int j = i + 1; j < sessionCount; j++)
                if (conflictGraph[i][j] == 1)
                    edgeCount++;
        report << "Conflict Edges: " << edgeCount << "\n";

        report.close();

        cout << "\n=== Module 3: Logic Validation Complete ===\n";
        if (allRulesPassed) {
            cout << "[PASS] Result: All logical constraints satisfied.\n";
        }
        else {
            cout << "[FAIL] Result: " << violationCount << " violation(s) found.\n";
        }
        cout << "Report saved to: logic_validation.txt\n\n";
    }

    void buildGraph() {
        for (int i = 0; i < sessionCount; i++)
            for (int j = 0; j < sessionCount; j++)
                graph[i][j] = 0;

        for (int i = 0; i < classCount && i < MAX; i++) {
            for (int j = 0; j < classCount && j < MAX; j++) {
                if (i != j && distanceMatrix[i][j] > 0) {
                    graph[i][j] = 1;
                }
            }
        }
    }

    // MODULE 5: Union-Find for Kruskal's
    int findSet(int i) {
        if (parent[i] == i)
            return i;
        return parent[i] = findSet(parent[i]);
    }

    void unionSets(int u, int v) {
        int pu = findSet(u);
        int pv = findSet(v);
        parent[pu] = pv;
    }

    // MODULE 5: Kruskal's MST
    void kruskalMST() {
        Edge edges[MAX * MAX];
        int edgeCount = 0;

        for (int i = 0; i < classCount && i < MAX; i++) {
            for (int j = i + 1; j < classCount && j < MAX; j++) {
                if (distanceMatrix[i][j] > 0) {
                    edges[edgeCount].u = i;
                    edges[edgeCount].v = j;
                    edges[edgeCount].weight = distanceMatrix[i][j];
                    edgeCount++;
                }
            }
        }

        for (int i = 0; i < edgeCount - 1; i++) {
            for (int j = i + 1; j < edgeCount; j++) {
                if (edges[i].weight > edges[j].weight) {
                    Edge temp = edges[i];
                    edges[i] = edges[j];
                    edges[j] = temp;
                }
            }
        }

        for (int i = 0; i < classCount; i++)
            parent[i] = i;

        int mstWeight = 0;
        ofstream fout("kruskal_mst.txt");
        fout << "=== Kruskal's MST Algorithm ===\n\n";
        fout << "Optimal Routes (Minimum Spanning Tree):\n";
        fout << "----------------------------------------\n";

        for (int i = 0; i < edgeCount; i++) {
            int uSet = findSet(edges[i].u);
            int vSet = findSet(edges[i].v);
            if (uSet != vSet) {
                mstWeight += edges[i].weight;
                fout << sessions[edges[i].u].className << " <-> "
                    << sessions[edges[i].v].className << " | Distance: "
                    << edges[i].weight << " units\n";
                unionSets(uSet, vSet);
            }
        }
        fout << "\n----------------------------------------\n";
        fout << "Total MST Weight: " << mstWeight << " units\n";
        fout.close();
        cout << "Module 5: Kruskal's MST completed. Total Distance: " << mstWeight << "\n";
    }

    // MODULE 5: Prim's MST
    void primMST() {
        bool selected[MAX] = { false };
        int key[MAX];
        int parentPrim[MAX];
        const int INF = 1000000;

        for (int i = 0; i < classCount && i < MAX; i++) {
            key[i] = INF;
            parentPrim[i] = -1;
        }

        key[0] = 0;
        int totalMSTDistance = 0;

        for (int count = 0; count < classCount - 1; count++) {
            int minKey = INF, u = -1;
            for (int v = 0; v < classCount && v < MAX; v++)
                if (!selected[v] && key[v] < minKey) {
                    minKey = key[v];
                    u = v;
                }

            if (u == -1) break;
            selected[u] = true;

            for (int v = 0; v < classCount && v < MAX; v++) {
                if (!selected[v] && distanceMatrix[u][v] > 0 &&
                    distanceMatrix[u][v] < key[v]) {
                    key[v] = distanceMatrix[u][v];
                    parentPrim[v] = u;
                }
            }
        }

        ofstream fout("prim_mst.txt");
        fout << "=== Prim's MST Algorithm ===\n\n";
        fout << "Optimal Routes (Minimum Spanning Tree):\n";
        fout << "----------------------------------------\n";
        for (int i = 1; i < classCount && i < MAX; i++) {
            if (parentPrim[i] != -1) {
                fout << sessions[parentPrim[i]].className << " <-> "
                    << sessions[i].className << " | Distance: "
                    << distanceMatrix[i][parentPrim[i]] << " units\n";
                totalMSTDistance += distanceMatrix[i][parentPrim[i]];
            }
        }
        fout << "\n----------------------------------------\n";
        fout << "Total MST Weight: " << totalMSTDistance << " units\n";
        fout.close();
        cout << "Module 5: Prim's MST completed. Total Distance: " << totalMSTDistance << "\n";
    }

    // MODULE 6: Euler Path
    int degreeEuler(int v) {
        if (v >= sessionCount) return 0;
        int deg = 0;
        for (int i = 0; i < sessionCount; i++)
            if (graph[v][i] > 0)
                deg++;
        return deg;
    }

    bool hasEulerPath() {
        int oddCount = 0;
        for (int i = 0; i < sessionCount; i++)
            if (degreeEuler(i) % 2 != 0)
                oddCount++;
        return (oddCount == 0 || oddCount == 2);
    }

    void findEulerPath() {
        ofstream fout("euler_path.txt");
        fout << "=== Euler Path Analysis ===\n\n";

        if (!hasEulerPath()) {
            fout << "Result: Euler Path does not exist for this graph.\n";
            fout << "Reason: Graph has more than 2 vertices with odd degree.\n";
            fout.close();
            eulerIndex = 0;
            cout << "Module 6: Euler Path - Does not exist\n";
            return;
        }

        int tempGraph[MAX][MAX];
        for (int i = 0; i < sessionCount; i++)
            for (int j = 0; j < sessionCount; j++)
                tempGraph[i][j] = graph[i][j];

        stack<int> currPath;
        eulerIndex = 0;
        currPath.push(0);

        while (!currPath.empty()) {
            int v = currPath.top();
            int i;
            for (i = 0; i < sessionCount; i++)
                if (tempGraph[v][i] > 0)
                    break;

            if (i == sessionCount) {
                eulerPath[eulerIndex++] = v;
                currPath.pop();
            }
            else {
                currPath.push(i);
                tempGraph[v][i]--;
                tempGraph[i][v]--;
            }
        }

        fout << "Euler Path Found:\n";
        fout << "----------------------------------------\n";
        for (int i = eulerIndex - 1; i >= 0; i--) {
            fout << sessions[eulerPath[i]].className;
            if (i != 0)
                fout << " -> ";
        }
        fout << "\n\n(Visits all edges exactly once)\n";
        fout.close();
        cout << "Module 6: Euler Path - Found and saved\n";
    }

    // MODULE 6: Hamiltonian Path
    bool hamiltonUtil(int path[], bool visited[], int pos) {
        if (pos == sessionCount)
            return true;

        for (int v = 0; v < sessionCount; v++) {
            if (!visited[v] && path[pos - 1] >= 0 && path[pos - 1] < sessionCount && graph[path[pos - 1]][v]) {
                path[pos] = v;
                visited[v] = true;
                if (hamiltonUtil(path, visited, pos + 1))
                    return true;
                visited[v] = false;
            }
        }
        return false;
    }

    void findHamiltonianPath() {
        bool visited[MAX];
        for (int i = 0; i < sessionCount && i < MAX; i++) {
            visited[i] = false;
            hamiltonPath[i] = -1;
        }

        bool found = false;
        for (int start = 0; start < sessionCount; start++) {
            hamiltonPath[0] = start;
            visited[start] = true;
            if (hamiltonUtil(hamiltonPath, visited, 1)) {
                found = true;
                break;
            }
            visited[start] = false;
        }

        ofstream fout("hamilton_path.txt");
        fout << "=== Hamiltonian Path Analysis ===\n\n";
        if (found) {
            fout << "Hamiltonian Path Found:\n";
            fout << "----------------------------------------\n";
            for (int i = 0; i < sessionCount; i++) {
                fout << sessions[hamiltonPath[i]].className;
                if (i != sessionCount - 1)
                    fout << " -> ";
            }
            fout << "\n\n(Visits all vertices exactly once)\n";
            cout << "Module 6: Hamiltonian Path - Found and saved\n";
        }
        else {
            fout << "Result: Hamiltonian Path does not exist for this graph.\n";
            fout << "Reason: No path visits all vertices exactly once.\n";
            cout << "Module 6: Hamiltonian Path - Does not exist\n";
        }
        fout.close();
    }
};

// ==================== CLASS: SCHEDULER (MODULES 4, 7) ====================
class Scheduler {
public:
    // MODULE 4: Graph Coloring
    bool isSafeColor(int vertex, int color) {
        if (vertex >= sessionCount) return false;
        for (int i = 0; i < sessionCount; i++) {
            if (conflictGraph[vertex][i] == 1 && sessionColor[i] == color)
                return false;
        }
        return true;
    }

    bool colorGraphUtil(int vertex, int maxColors) {
        if (vertex >= sessionCount)
            return true;
        if (vertex < 0)
            return false;

        for (int color = 0; color < maxColors && color < slotCount; color++) {
            if (isSafeColor(vertex, color)) {
                sessionColor[vertex] = color;
                if (colorGraphUtil(vertex + 1, maxColors))
                    return true;
                sessionColor[vertex] = -1;
            }
        }
        return false;
    }

    void assignTimeslotsGraphColoring() {
        cout << "\n=== MODULE 4: Graph Coloring for Timetable ===\n";
        for (int i = 0; i < sessionCount && i < MAX; i++)
            sessionColor[i] = -1;

        int maxColors = slotCount;
        bool coloringSuccess = colorGraphUtil(0, maxColors);

        if (!coloringSuccess) {
            cout << "Error: Cannot assign timeslots without conflicts!\n";
            cout << "Need more time slots or reduce conflicts.\n\n";
            return;
        }

        for (int i = 0; i < sessionCount && i < MAX; i++) {
            if (sessionColor[i] >= 0 && sessionColor[i] < slotCount) {
                sessionTime[i] = timeSlots[sessionColor[i]].slot;
            }
        }

        cout << "[SUCCESS] Graph coloring completed successfully\n";
        cout << "[SUCCESS] Time slots assigned without conflicts\n\n";
    }

    // MODULE 7: Recursive Scheduling
    bool canAssignSlot(int session, const string& slot) {
        if (session >= sessionCount) return false;
        for (int i = 0; i < sessionCount; i++) {
            if (conflictGraph[session][i] && sessionTime[i] == slot) {
                return false;
            }
        }
        return true;
    }

    bool scheduleRecursive(int index) {
        if (index == sessionCount)
            return true;

        for (int i = 0; i < slotCount && i < MAX; i++) {
            if (canAssignSlot(index, timeSlots[i].slot)) {
                sessionTime[index] = timeSlots[i].slot;
                if (scheduleRecursive(index + 1))
                    return true;
                sessionTime[index] = "";
            }
        }
        return false;
    }

    void recursiveScheduling() {
        cout << "=== MODULE 7: Recursive Backtracking Scheduling ===\n";
        for (int i = 0; i < sessionCount && i < MAX; i++)
            sessionTime[i] = "";

        if (scheduleRecursive(0)) {
            ofstream fout("recursive_schedule.txt");
            fout << "=== Recursive Scheduling Result ===\n\n";
            fout << "Class | Teacher | Room | Time Slot\n";
            fout << "----------------------------------------\n";
            for (int i = 0; i < sessionCount; i++) {
                fout << sessions[i].className << " | " << sessions[i].teacher
                    << " | " << sessions[i].room << " | " << sessionTime[i] << "\n";
            }
            fout.close();
            cout << "[SUCCESS] Recursive scheduling completed successfully.\n";
            cout << "Results saved to: recursive_schedule.txt\n\n";
        }
        else {
            ofstream fout("recursive_schedule.txt");
            fout << "No valid schedule found using recursive backtracking.\n";
            fout.close();
            cout << "[FAIL] No valid schedule found.\n\n";
        }
    }
};

// ==================== CLASS: OUTPUT MANAGER (MODULE 8) ====================
class OutputManager {
public:
    void displayTimetable() {
        cout << "\n=============================================\n";
        cout << "     FINAL TIMETABLE GENERATED\n";
        cout << "=============================================\n\n";
        cout << "Class | Subject | Teacher | Room | Time Slot\n";
        cout << "---------------------------------------------\n";

        for (int i = 0; i < sessionCount && i < MAX; i++) {
            cout << sessions[i].className << " | " << sessions[i].subject
                << " | " << sessions[i].teacher << " | " << sessions[i].room
                << " | " << sessionTime[i] << "\n";
        }
        cout << "\n";
    }

    void saveTimetable() {
        ofstream timetableFile("timetable.txt");
        timetableFile << "=== SMART TIMETABLE GENERATOR ===\n\n";
        timetableFile << "Class | Subject | Teacher | Room | Time Slot\n";
        timetableFile << "---------------------------------------------\n";
        for (int i = 0; i < sessionCount; i++) {
            timetableFile << sessions[i].className << " | " << sessions[i].subject
                << " | " << sessions[i].teacher << " | " << sessions[i].room
                << " | " << sessionTime[i] << "\n";
        }
        timetableFile.close();
        cout << "Timetable saved to: timetable.txt\n";
    }

    void validateSchedule() {
        ofstream validationFile("validation_report.txt");
        validationFile << "=== SCHEDULE VALIDATION REPORT ===\n\n";

        bool conflictsFound = false;
        int conflictCount = 0;

        for (int i = 0; i < sessionCount; i++) {
            for (int j = i + 1; j < sessionCount; j++) {
                bool sameTeacher = sessions[i].teacher == sessions[j].teacher &&
                    sessions[i].teacher != "Unknown";
                bool sameRoom = sessions[i].room == sessions[j].room;
                bool sameTime = sessionTime[i] == sessionTime[j];

                if (sameTeacher && sameTime) {
                    validationFile << "CONFLICT: " << sessions[i].className
                        << " & " << sessions[j].className
                        << " - Same teacher at same time\n";
                    conflictsFound = true;
                    conflictCount++;
                }

                if (sameRoom && sameTime) {
                    validationFile << "CONFLICT: " << sessions[i].className
                        << " & " << sessions[j].className
                        << " - Same room at same time\n";
                    conflictsFound = true;
                    conflictCount++;
                }
            }
        }

        if (!conflictsFound) {
            validationFile << "[PASS] VALIDATION: PASS\n";
            validationFile << "No conflicts found. Schedule is valid.\n";
            cout << "\n[PASS] Validation: PASS - No conflicts found.\n";
        }
        else {
            validationFile << "[FAIL] VALIDATION: FAIL\n";
            validationFile << "Total conflicts found: " << conflictCount << "\n";
            cout << "\n[FAIL] Validation: FAIL - " << conflictCount << " conflict(s) found!\n";
        }

        validationFile.close();
        cout << "Validation report saved to: validation_report.txt\n";
    }

    void displayAllReports() {
        cout << "\n=============================================\n";
        cout << "     ALL REPORTS GENERATED\n";
        cout << "=============================================\n\n";
        cout << "Output Files Created:\n";
        cout << "1. timetable.txt - Final class schedule\n";
        cout << "2. validation_report.txt - Schedule validation\n";
        cout << "3. logic_validation.txt - Logic rule checking\n";
        cout << "4. prim_mst.txt - Prim's MST routes\n";
        cout << "5. kruskal_mst.txt - Kruskal's MST routes\n";
        cout << "6. euler_path.txt - Euler path analysis\n";
        cout << "7. hamilton_path.txt - Hamiltonian path analysis\n";
        cout << "8. recursive_schedule.txt - Recursive scheduling\n";
        cout << "\n=============================================\n";
    }
};

// ==================== MAIN FUNCTION ====================
int main() {
    cout << "\n=============================================\n";
    cout << "  SMART TIMETABLE GENERATOR & ROUTE PLANNER\n";
    cout << "=============================================\n";
    cout << "  Using Discrete Mathematics Concepts\n";
    cout << "=============================================\n";

    // MODULE 1: Data Input & Initialization
    FileManager fileManager;
    fileManager.initializeSystem();

    // MODULE 2: Build Sessions and Graphs
    GraphManager graphManager;
    graphManager.buildSessions();
    graphManager.buildConflictGraph();
    graphManager.buildGraph();

    // MODULE 4: Graph Coloring for Timetable Assignment
    Scheduler scheduler;
    scheduler.assignTimeslotsGraphColoring();

    // MODULE 3: Logic Validation (AFTER scheduling)
    graphManager.generateValidationReport();

    // MODULE 7: Recursive Scheduling (Alternative)
    scheduler.recursiveScheduling();

    // MODULE 5: Minimum Spanning Tree Algorithms
    graphManager.primMST();
    graphManager.kruskalMST();

    // MODULE 6: Euler and Hamiltonian Paths
    graphManager.findEulerPath();
    graphManager.findHamiltonianPath();
   
    // MODULE 8: Output & Display
    OutputManager outputManager;
    outputManager.displayTimetable();
    outputManager.saveTimetable();
    outputManager.validateSchedule();

    // ----------- Display MST Total Distance on Console -----------
    ifstream mstFile("prim_mst.txt");
    string line;
    int totalMSTDistance = 0;
    if (mstFile) {
        while (getline(mstFile, line)) {
            if (line.find("Total MST Weight:") != string::npos) {
                size_t colonPos = line.find(":");
                if (colonPos != string::npos) {
                    string numStr = line.substr(colonPos + 1);
                    // Remove " units" if present
                    size_t unitsPos = numStr.find(" units");
                    if (unitsPos != string::npos)
                        numStr = numStr.substr(0, unitsPos);
                    totalMSTDistance = stoi(numStr);
                }
            }
        }
        mstFile.close();
    }
    cout << "MST Total Distance: " << totalMSTDistance << "\n";

    // ----------- Create mst.txt (copy from prim_mst.txt) -----------
    ofstream mstFileOut("mst.txt");
    ifstream primFileRead("prim_mst.txt");
    if (primFileRead) {
        string line;
        while (getline(primFileRead, line)) {
            mstFileOut << line << "\n";
        }
        primFileRead.close();
    }
    mstFileOut.close();

    // ----------- Create shortest_paths.txt (Distance Matrix) -----------
    ofstream shortestFile("shortest_paths.txt");
    shortestFile << "=== Distance Matrix ===\n\n";
    shortestFile << "Distances between classrooms:\n";
    shortestFile << "-----------------------------\n";
    for (int i = 0; i < classCount; i++) {
        for (int j = 0; j < classCount; j++) {
            shortestFile << distanceMatrix[i][j] << " ";
        }
        shortestFile << "\n";
    }
    shortestFile.close();

    // ----------- Display and Save Euler Path -----------
    cout << "Euler Path: ";
    ofstream eulerFileOut("euler_path.txt");
    eulerFileOut << "=== Euler Path ===\n\n";
    if (eulerIndex == 0) {
        cout << "N/A\n";
        eulerFileOut << "Result: Euler Path does not exist.\n";
    }
    else {
        eulerFileOut << "Euler Path Found:\n";
        for (int i = eulerIndex - 1; i >= 0; i--) {
            cout << sessions[eulerPath[i]].className;
            eulerFileOut << sessions[eulerPath[i]].className;
            if (i != 0) {
                cout << " -> ";
                eulerFileOut << " -> ";
            }
        }
        cout << "\n";
        eulerFileOut << "\n";
    }
    eulerFileOut.close();

    // ----------- Display and Save Hamilton Path -----------
    cout << "Hamilton Path: ";
    ofstream hamiltonFileOut("hamilton_path.txt");
    hamiltonFileOut << "=== Hamiltonian Path ===\n\n";
    if (hamiltonPath[0] == -1) {
        cout << "N/A\n";
        hamiltonFileOut << "Result: Hamiltonian Path does not exist.\n";
    }
    else {
        hamiltonFileOut << "Hamiltonian Path Found:\n";
        for (int i = 0; i < sessionCount; i++) {
            cout << sessions[hamiltonPath[i]].className;
            hamiltonFileOut << sessions[hamiltonPath[i]].className;
            if (i != sessionCount - 1) {
                cout << " -> ";
                hamiltonFileOut << " -> ";
            }
        }
        cout << "\n";
        hamiltonFileOut << "\n";
    }
    hamiltonFileOut.close();

    outputManager.displayAllReports();

    cout << "\n[SUCCESS] System execution completed successfully!\n\n";
    system("pause");
    return 0;
}