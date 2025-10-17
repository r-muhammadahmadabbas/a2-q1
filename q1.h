#pragma once

#include <iostream>
#include <fstream>

// ============ Custom String Class (No <string> allowed) ============
class String {
private:
    char* data;
    int len;

    void copyFrom(const char* str) {
        if (str) {
            len = 0;
            while (str[len]) len++;
            data = new char[len + 1];
            for (int i = 0; i <= len; i++) data[i] = str[i];
        }
        else {
            data = nullptr;
            len = 0;
        }
    }

public:
    String() : data(nullptr), len(0) {}

    String(const char* str) {
        copyFrom(str);
    }

    String(const String& other) : len(other.len) {
        if (other.data) {
            data = new char[len + 1];
            for (int i = 0; i <= len; i++) data[i] = other.data[i];
        }
        else {
            data = nullptr;
        }
    }

    ~String() {
        delete[] data;
    }

    String& operator=(const String& other) {
        if (this != &other) {
            delete[] data;
            len = other.len;
            if (other.data) {
                data = new char[len + 1];
                for (int i = 0; i <= len; i++) data[i] = other.data[i];
            }
            else {
                data = nullptr;
            }
        }
        return *this;
    }

    String& operator=(const char* str) {
        delete[] data;
        copyFrom(str);
        return *this;
    }

    const char* c_str() const {
        return data ? data : "";
    }

    int length() const {
        return len;
    }

    bool operator==(const String& other) const {
        if (len != other.len) return false;
        for (int i = 0; i < len; i++) {
            if (data[i] != other.data[i]) return false;
        }
        return true;
    }

    bool operator==(const char* str) const {
        if (!data && !str) return true;
        if (!data || !str) return false;
        int i = 0;
        while (data[i] && str[i]) {
            if (data[i] != str[i]) return false;
            i++;
        }
        return data[i] == str[i];
    }
};

// ============ Student Struct ============
struct Student {
    int rollNumber;
    int batchID;
    String subject;

    Student* next;

    Student() : rollNumber(0), batchID(0), next(nullptr) {}

    Student(int roll, int batch, const char* subj)
        : rollNumber(roll), batchID(batch), subject(subj), next(nullptr) {
    }
};

// ============ Seat Node ============
struct Seat {
    Student* student;

    // 8 directional pointers
    Seat* front;
    Seat* back;
    Seat* left;
    Seat* right;
    Seat* up;
    Seat* down;
    Seat* leftBuilding;
    Seat* rightBuilding;

    int row;
    int col;

    Seat() : student(nullptr), front(nullptr), back(nullptr),
        left(nullptr), right(nullptr), up(nullptr), down(nullptr),
        leftBuilding(nullptr), rightBuilding(nullptr), row(0), col(0) {
    }

    Seat(int r, int c) : student(nullptr), front(nullptr), back(nullptr),
        left(nullptr), right(nullptr), up(nullptr), down(nullptr),
        leftBuilding(nullptr), rightBuilding(nullptr), row(r), col(c) {
    }
};

// ============ Room Struct ============
struct Room {
    int roomNumber;
    Seat* grid;
    int gridSize;
    int occupiedSeats;

    Room* next;
    Room* prev;

    Room() : roomNumber(0), grid(nullptr), gridSize(0),
        occupiedSeats(0), next(nullptr), prev(nullptr) {
    }

    Room(int roomNo, int size) : roomNumber(roomNo), gridSize(size),
        occupiedSeats(0), next(nullptr), prev(nullptr) {
        // Create grid seats
        Seat* prevRow = nullptr;
        for (int r = 0; r < gridSize; r++) {
            Seat* rowStart = nullptr;
            Seat* prev = nullptr;
            for (int c = 0; c < gridSize; c++) {
                Seat* seat = new Seat(r, c);

                if (c == 0) rowStart = seat;
                if (r == 0 && c == 0) grid = seat;

                if (prev) {
                    prev->right = seat;
                    seat->left = prev;
                }

                if (prevRow) {
                    Seat* temp = prevRow;
                    for (int k = 0; k < c; k++) temp = temp->right;
                    temp->back = seat;
                    seat->front = temp;
                }

                prev = seat;
            }
            prevRow = rowStart;
        }
    }

    //~Room() {
    //    Seat* rowPtr = grid;
    //    while (rowPtr) {
    //        Seat* nextRow = rowPtr->back;
    //        Seat* seat = rowPtr;
    //        while (seat) {
    //            Seat* nextSeat = seat->right;
    //            if (seat->student) delete seat->student;
    //            delete seat;
    //            seat = nextSeat;
    //        }
    //        rowPtr = nextRow;
    //    }
    //}

    ~Room() {
        // Return early if the grid was never allocated.
        if (!grid) {
            return;
        }

        Seat* rowPtr = grid;
        // Loop exactly 'gridSize' times to cover each row in this room.
        for (int r = 0; r < gridSize; ++r) {
            // It's possible the grid is malformed; break if a row is missing.
            if (!rowPtr) {
                break;
            }

            // The 'back' pointers are safe because they aren't rewired between rooms.
            // Save the start of the next row before we start deleting the current one.
            Seat* nextRow = rowPtr->back;

            Seat* seat = rowPtr;
            // Loop exactly 'gridSize' times to cover each seat in this row.
            for (int c = 0; c < gridSize; ++c) {
                // Break if the row is shorter than expected.
                if (!seat) {
                    break;
                }

                // Save the next seat's pointer before deleting the current one.
                Seat* nextSeat = seat->right;

                if (seat->student) {
                    delete seat->student;
                }
                delete seat;

                seat = nextSeat;
            }

            rowPtr = nextRow;
        }
        // Prevent dangling pointer issues.
        grid = nullptr;
    }

    Seat* getSeat(int row, int col) {
        if (!grid || row < 0 || row >= gridSize || col < 0 || col >= gridSize) return nullptr;

        Seat* current = grid;
        for (int r = 0; r < row; r++) {
            if (!current) return nullptr;
            current = current->back;
        }
        for (int c = 0; c < col; c++) {
            if (!current) return nullptr;
            current = current->right;
        }
        return current;
    }

    bool isFull() {
        return occupiedSeats >= (gridSize * gridSize);
    }

    int getMinOccupancy() {
        int total = gridSize * gridSize;
        return (total + 1) / 2;
    }

    bool isHalfFull() {
        return occupiedSeats >= getMinOccupancy();
    }
};

// ============ Floor Struct ============
struct Floor {
    int floorNumber;
    Room* firstRoom;
    Room* lastRoom;
    int totalRooms;

    Floor* next;
    Floor* prev;

    Floor() : floorNumber(0), firstRoom(nullptr), lastRoom(nullptr),
        totalRooms(0), next(nullptr), prev(nullptr) {
    }

    Floor(int floorNo, int numRooms, int gridSize)
        : floorNumber(floorNo), firstRoom(nullptr), lastRoom(nullptr),
        totalRooms(numRooms), next(nullptr), prev(nullptr) {

        Room* prevRoom = nullptr;
        for (int i = 1; i <= numRooms; i++) {
            Room* room = new Room(i, gridSize);

            if (!firstRoom) firstRoom = room;
            if (prevRoom) {
                prevRoom->next = room;
                room->prev = prevRoom;
            }
            prevRoom = room;
        }
        lastRoom = prevRoom;
    }

    ~Floor() {
        Room* current = firstRoom;
        while (current) {
            Room* temp = current;
            current = current->next;
            delete temp;
        }
    }
};

// ============ Block Struct ============
struct Block {
    char blockID;
    Floor* firstFloor;
    Floor* lastFloor;
    int totalFloors;

    Block* next;
    Block* prev;

    Block() : blockID('A'), firstFloor(nullptr), lastFloor(nullptr),
        totalFloors(0), next(nullptr), prev(nullptr) {
    }

    Block(char id, int numFloors, int roomsPerFloor, int gridSize)
        : blockID(id), firstFloor(nullptr), lastFloor(nullptr),
        totalFloors(numFloors), next(nullptr), prev(nullptr) {

        Floor* prevFloor = nullptr;
        for (int i = 1; i <= numFloors; i++) {
            Floor* floor = new Floor(i, roomsPerFloor, gridSize);

            if (!firstFloor) firstFloor = floor;
            if (prevFloor) {
                prevFloor->next = floor;
                floor->prev = prevFloor;
            }
            prevFloor = floor;
        }
        lastFloor = prevFloor;
    }

    ~Block() {
        Floor* current = firstFloor;
        while (current) {
            Floor* temp = current;
            current = current->next;
            delete temp;
        }
    }
};

// ============ Seating System Class ============
class SeatingSystem {
private:
    Block* firstBlock;
    Block* lastBlock;
    int gridSize;
    int totalBlocks;

    // Helper to find block
    Block* findBlock(char blockID) {
        Block* current = firstBlock;
        while (current) {
            if (current->blockID == blockID) return current;
            current = current->next;
        }
        return nullptr;
    }

    // Helper to find floor
    Floor* findFloor(Block* block, int floorNo) {
        if (!block) return nullptr;
        Floor* current = block->firstFloor;
        while (current) {
            if (current->floorNumber == floorNo) return current;
            current = current->next;
        }
        return nullptr;
    }

    // Helper to find room
    Room* findRoom(Floor* floor, int roomNo) {
        if (!floor) return nullptr;
        Room* current = floor->firstRoom;
        while (current) {
            if (current->roomNumber == roomNo) return current;
            current = current->next;
        }
        return nullptr;
    }

    // Get room from seat (reverse lookup)
    Room* getRoomFromSeat(Seat* seat, Block** outBlock, Floor** outFloor) {
        if (!seat) return nullptr;

        Block* block = firstBlock;
        while (block) {
            Floor* floor = block->firstFloor;
            while (floor) {
                Room* room = floor->firstRoom;
                while (room) {
                    for (int r = 0; r < gridSize; r++) {
                        for (int c = 0; c < gridSize; c++) {
                            if (room->getSeat(r, c) == seat) {
                                if (outBlock) *outBlock = block;
                                if (outFloor) *outFloor = floor;
                                return room;
                            }
                        }
                    }
                    room = room->next;
                }
                floor = floor->next;
            }
            block = block->next;
        }
        return nullptr;
    }

    // Check batch parity
    bool matchesParity(int batchID, int col) {
        bool oddBatch = (batchID % 2 == 1);
        bool oddCol = (col % 2 == 1);
        return oddBatch == oddCol;
    }

    // Count occupied seats in parity columns for a batch
    int countOccupiedInParity(Room* room, int batchID) {
        int count = 0;
        bool oddBatch = (batchID % 2 == 1);

        for (int col = 0; col < gridSize; col++) {
            bool oddCol = (col % 2 == 1);
            if (oddBatch != oddCol) continue;

            for (int row = 0; row < gridSize; row++) {
                Seat* seat = room->getSeat(row, col);
                if (seat && seat->student && seat->student->batchID == batchID) {
                    count++;
                }
            }
        }
        return count;
    }

    // Count different subjects in parity columns for a batch
    int countSubjectsInParity(Room* room, int batchID, const char* excludeSubject) {
        String subjects[10];
        int subjectCount = 0;
        bool oddBatch = (batchID % 2 == 1);

        for (int col = 0; col < gridSize; col++) {
            bool oddCol = (col % 2 == 1);
            if (oddBatch != oddCol) continue;

            for (int row = 0; row < gridSize; row++) {
                Seat* seat = room->getSeat(row, col);
                if (seat && seat->student && seat->student->batchID == batchID) {
                    if (!(seat->student->subject == excludeSubject)) {
                        bool found = false;
                        for (int i = 0; i < subjectCount; i++) {
                            if (subjects[i] == seat->student->subject.c_str()) {
                                found = true;
                                break;
                            }
                        }
                        if (!found && subjectCount < 10) {
                            subjects[subjectCount] = seat->student->subject.c_str();
                            subjectCount++;
                        }
                    }
                }
            }
        }
        return subjectCount;
    }

    // Check subject restriction
    bool checkSubjectRestriction(Room* room, int batchID, const char* subject) {
        int parityOccupied = countOccupiedInParity(room, batchID);
        int maxParitySeats = (gridSize * gridSize) / 2;

        if (parityOccupied > maxParitySeats / 2) {
            int existingSubjects = countSubjectsInParity(room, batchID, subject);
            if (existingSubjects > 0) {
                return false;
            }
        }
        return true;
    }

    // Find insertion position
    Seat* findInsertionPosition(int rollNo, int batchID, const char* subject,
        Block** outBlock, Floor** outFloor, Room** outRoom) {
        Block* block = firstBlock;

        while (block) {
            Floor* floor = block->firstFloor;
            while (floor) {
                Room* room = floor->firstRoom;
                while (room) {
                    // Column-major traversal
                    for (int col = 0; col < gridSize; col++) {
                        if (!matchesParity(batchID, col)) continue;

                        for (int row = 0; row < gridSize; row++) {
                            Seat* seat = room->getSeat(row, col);
                            if (!seat) continue;

                            if (!seat->student) {
                                if (checkSubjectRestriction(room, batchID, subject)) {
                                    if (outBlock) *outBlock = block;
                                    if (outFloor) *outFloor = floor;
                                    if (outRoom) *outRoom = room;
                                    return seat;
                                }
                            }
                            else if (rollNo < seat->student->rollNumber) {
                                if (outBlock) *outBlock = block;
                                if (outFloor) *outFloor = floor;
                                if (outRoom) *outRoom = room;
                                return seat;
                            }
                        }
                    }
                    room = room->next;
                }
                floor = floor->next;
            }
            block = block->next;
        }
        return nullptr;
    }

    // Connect vertical pointers (up/down)
    void connectVertically() {
        Block* block = firstBlock;
        while (block) {
            Floor* floor = block->firstFloor;
            Floor* floorAbove = nullptr;

            while (floor) {
                if (floorAbove) {
                    Room* room = floor->firstRoom;
                    Room* roomAbove = floorAbove->firstRoom;

                    while (room && roomAbove) {
                        for (int r = 0; r < gridSize; r++) {
                            for (int c = 0; c < gridSize; c++) {
                                Seat* seat = room->getSeat(r, c);
                                Seat* seatAbove = roomAbove->getSeat(r, c);
                                if (seat && seatAbove) {
                                    seat->up = seatAbove;
                                    seatAbove->down = seat;
                                }
                            }
                        }
                        room = room->next;
                        roomAbove = roomAbove->next;
                    }
                }
                floorAbove = floor;
                floor = floor->next;
            }
            block = block->next;
        }
    }

    // Connect cross-block pointers
    void connectCrossBlock() {
        Block* block = firstBlock;
        while (block && block->next) {
            Block* nextBlock = block->next;

            Floor* floor1 = block->firstFloor;
            Floor* floor2 = nextBlock->firstFloor;

            while (floor1 && floor2) {
                Room* room1 = floor1->firstRoom;
                Room* room2 = floor2->firstRoom;

                while (room1 && room2) {
                    for (int r = 0; r < gridSize; r++) {
                        Seat* rightmost = room1->getSeat(r, gridSize - 1);
                        Seat* leftmost = room2->getSeat(r, 0);

                        if (rightmost && leftmost) {
                            rightmost->rightBuilding = leftmost;
                            leftmost->leftBuilding = rightmost;
                        }
                    }
                    room1 = room1->next;
                    room2 = room2->next;
                }
                floor1 = floor1->next;
                floor2 = floor2->next;
            }
            block = block->next;
        }
    }

    // Connect floor continuity
    void connectFloorContinuity() {
        Block* block = firstBlock;
        while (block) {
            Floor* floor = block->firstFloor;
            while (floor && floor->next) {
                Room* lastRoom = floor->lastRoom;
                Room* firstRoomNext = floor->next->firstRoom;

                if (lastRoom && firstRoomNext) {
                    for (int r = 0; r < gridSize; r++) {
                        Seat* lastCol = lastRoom->getSeat(r, gridSize - 1);
                        Seat* firstCol = firstRoomNext->getSeat(r, 0);

                        if (lastCol && firstCol) {
                            lastCol->right = firstCol;
                            firstCol->left = lastCol;
                        }
                    }
                }
                floor = floor->next;
            }

            // Connect top floor of current block to bottom floor of next block
            if (block->next) {
                Room* topLastRoom = block->lastFloor->lastRoom;
                Room* nextBottomFirst = block->next->firstFloor->firstRoom;

                if (topLastRoom && nextBottomFirst) {
                    for (int r = 0; r < gridSize; r++) {
                        Seat* lastSeat = topLastRoom->getSeat(r, gridSize - 1);
                        Seat* firstSeat = nextBottomFirst->getSeat(r, 0);

                        if (lastSeat && firstSeat) {
                            lastSeat->right = firstSeat;
                            firstSeat->left = lastSeat;
                        }
                    }
                }
            }
            block = block->next;
        }
    }

public:
    SeatingSystem() : firstBlock(nullptr), lastBlock(nullptr), gridSize(4), totalBlocks(0) {}

    SeatingSystem(int numBlocks, int floorsPerBlock, int roomsPerFloor, int gridSize)
        : firstBlock(nullptr), lastBlock(nullptr), gridSize(gridSize), totalBlocks(numBlocks) {

        char blockIDs[3] = { 'A', 'B', 'C' };
        Block* prevBlock = nullptr;

        for (int i = 0; i < numBlocks; i++) {
            Block* block = new Block(blockIDs[i], floorsPerBlock, roomsPerFloor, gridSize);

            if (!firstBlock) firstBlock = block;
            if (prevBlock) {
                prevBlock->next = block;
                block->prev = prevBlock;
            }
            prevBlock = block;
        }
        lastBlock = prevBlock;

        connectVertically();
        connectCrossBlock();
        connectFloorContinuity();
    }

    ~SeatingSystem() {
        Block* current = firstBlock;
        while (current) {
            Block* temp = current;
            current = current->next;
            delete temp;
        }
    }

    // Insert student
    void insertStudent(int rollNo, int batchID, const char* subject) {
        if (batchID < 22 || batchID > 25) {
            std::cout << "Invalid batch ID! Must be 22-25.\n";
            return;
        }

        Block* targetBlock = nullptr;
        Floor* targetFloor = nullptr;
        Room* targetRoom = nullptr;

        Seat* targetSeat = findInsertionPosition(rollNo, batchID, subject,
            &targetBlock, &targetFloor, &targetRoom);

        if (!targetSeat) {
            std::cout << "Cannot insert student - all seats full or constraints violated!\n";
            return;
        }

        Student* newStudent = new Student(rollNo, batchID, subject);

        if (!targetSeat->student) {
            targetSeat->student = newStudent;
            if (targetRoom) targetRoom->occupiedSeats++;
            std::cout << "Student " << rollNo << " inserted successfully.\n";
        }
        else {
            forwardCollapse(targetSeat, newStudent, targetRoom);
        }
    }

    // Forward collapse
    void forwardCollapse(Seat* startSeat, Student* newStudent, Room* startRoom) {
        if (!startSeat || !newStudent) return;

        if (!startSeat->student) {
            startSeat->student = newStudent;
            if (startRoom) startRoom->occupiedSeats++;
            return;
        }

        Student* displaced = startSeat->student;
        startSeat->student = newStudent;

        Block* block = nullptr;
        Floor* floor = nullptr;
        Room* room = nullptr;

        Seat* nextSeat = findInsertionPosition(displaced->rollNumber, displaced->batchID,
            displaced->subject.c_str(), &block, &floor, &room);

        if (!nextSeat) {
            std::cout << "Error: Cannot complete insertion!\n";
            startSeat->student = displaced;
            delete newStudent;
            return;
        }

        forwardCollapse(nextSeat, displaced, room);
    }

    // Delete student
    bool deleteStudent(int rollNo) {
        Block* block = firstBlock;

        while (block) {
            Floor* floor = block->firstFloor;
            while (floor) {
                Room* room = floor->firstRoom;
                while (room) {
                    for (int r = 0; r < gridSize; r++) {
                        for (int c = 0; c < gridSize; c++) {
                            Seat* seat = room->getSeat(r, c);
                            if (seat && seat->student && seat->student->rollNumber == rollNo) {
                                delete seat->student;
                                seat->student = nullptr;
                                room->occupiedSeats--;

                                backwardCollapse(seat, room);

                                if (room->occupiedSeats < room->getMinOccupancy()) {
                                    roomCollapse(room);
                                }

                                std::cout << "Student " << rollNo << " deleted successfully.\n";
                                return true;
                            }
                        }
                    }
                    room = room->next;
                }
                floor = floor->next;
            }
            block = block->next;
        }

        std::cout << "Student not found.\n";
        return false;
    }

    // Backward collapse
    void backwardCollapse(Seat* deletedSeat, Room* room) {
        // Simplified backward collapse - shifts next students backward
        Seat* current = deletedSeat;

        // Find next occupied seat in sequence
        Block* block = firstBlock;
        bool foundDeleted = false;

        while (block) {
            Floor* floor = block->firstFloor;
            while (floor) {
                Room* rm = floor->firstRoom;
                while (rm) {
                    for (int col = 0; col < gridSize; col++) {
                        for (int row = 0; row < gridSize; row++) {
                            Seat* seat = rm->getSeat(row, col);

                            if (!foundDeleted) {
                                if (seat == deletedSeat) foundDeleted = true;
                                continue;
                            }

                            if (seat && seat->student) {
                                current->student = seat->student;
                                seat->student = nullptr;
                                rm->occupiedSeats--;
                                if (room && current != deletedSeat) room->occupiedSeats++;
                                current = seat;
                                return;
                            }
                        }
                    }
                    rm = rm->next;
                }
                floor = floor->next;
            }
            block = block->next;
        }
    }

    // Room collapse (stub - borrow from previous room)
    void roomCollapse(Room* room) {
        std::cout << "Room collapse triggered (stub implementation).\n";
    }

    // Floor collapse
    void floorCollapse(Floor* floor) {
        std::cout << "Floor collapse triggered (stub implementation).\n";
    }

    // Block collapse
    void blockCollapse(Block* block) {
        std::cout << "Block collapse triggered (stub implementation).\n";
    }

    // Print room
    void printRoom(char blockID, int floorNo, int roomNo) {
        Block* block = findBlock(blockID);
        if (!block) {
            std::cout << "Block not found.\n";
            return;
        }

        Floor* floor = findFloor(block, floorNo);
        if (!floor) {
            std::cout << "Floor not found.\n";
            return;
        }

        Room* room = findRoom(floor, roomNo);
        if (!room) {
            std::cout << "Room not found.\n";
            return;
        }

        std::cout << "\n=== Block " << blockID << " - Floor " << floorNo
            << " - Room " << roomNo << " ===\n";

        for (int r = 0; r < gridSize; r++) {
            for (int c = 0; c < gridSize; c++) {
                Seat* seat = room->getSeat(r, c);
                if (seat && seat->student) {
                    std::cout << "[" << seat->student->rollNumber << "] ";
                }
                else {
                    std::cout << "[Empty] ";
                }
            }
            std::cout << "\n";
        }
        std::cout << "\n";
    }

    // Export seating plan
    void exportSeatingPlan(const char* fileName) {
        std::ofstream file(fileName);
        if (!file) {
            std::cout << "Cannot create file.\n";
            return;
        }

        Block* block = firstBlock;
        while (block) {
            Floor* floor = block->firstFloor;
            while (floor) {
                Room* room = floor->firstRoom;
                while (room) {
                    file << "Block " << block->blockID << " - Floor "
                        << floor->floorNumber << " - Room " << room->roomNumber << "\n";

                    for (int r = 0; r < gridSize; r++) {
                        for (int c = 0; c < gridSize; c++) {
                            Seat* seat = room->getSeat(r, c);
                            if (seat && seat->student) {
                                file << "[" << seat->student->rollNumber << "] ";
                            }
                            else {
                                file << "[Empty] ";
                            }
                        }
                        file << "\n";
                    }
                    file << "\n";
                    room = room->next;
                }
                floor = floor->next;
            }
            block = block->next;
        }

        file.close();
        std::cout << "Seating plan exported to " << fileName << "\n";
    }

    // Find student by roll
    Student* findStudentByRoll(int rollNo) {
        Block* block = firstBlock;
        while (block) {
            Floor* floor = block->firstFloor;
            while (floor) {
                Room* room = floor->firstRoom;
                while (room) {
                    for (int r = 0; r < gridSize; r++) {
                        for (int c = 0; c < gridSize; c++) {
                            Seat* seat = room->getSeat(r, c);
                            if (seat && seat->student && seat->student->rollNumber == rollNo) {
                                std::cout << "\n=== Student Found ===\n";
                                std::cout << "Roll Number: " << seat->student->rollNumber << "\n";
                                std::cout << "Batch: " << seat->student->batchID << "\n";
                                std::cout << "Subject: " << seat->student->subject.c_str() << "\n";
                                std::cout << "Location: Block " << block->blockID
                                    << ", Floor " << floor->floorNumber
                                    << ", Room " << room->roomNumber
                                    << ", Row " << r << ", Col " << c << "\n";
                                return seat->student;
                            }
                        }
                    }
                    room = room->next;
                }
                floor = floor->next;
            }
            block = block->next;
        }
        std::cout << "Student not found.\n";
        return nullptr;
    }

    // Find student by location
    Student* findStudentByLocation(char blockID, int floorNo, int roomNo, int row, int col) {
        Block* block = findBlock(blockID);
        if (!block) {
            std::cout << "Block not found.\n";
            return nullptr;
        }

        Floor* floor = findFloor(block, floorNo);
        if (!floor) {
            std::cout << "Floor not found.\n";
            return nullptr;
        }

        Room* room = findRoom(floor, roomNo);
        if (!room) {
            std::cout << "Room not found.\n";
            return nullptr;
        }

        Seat* seat = room->getSeat(row, col);
        if (!seat) {
            std::cout << "Invalid seat coordinates.\n";
            return nullptr;
        }

        if (seat->student) {
            std::cout << "\n=== Seat Occupied ===\n";
            std::cout << "Seat: Block " << blockID << ", Floor " << floorNo
                << ", Room " << roomNo << ", Row " << row << ", Col " << col << "\n";
            std::cout << "Roll Number: " << seat->student->rollNumber << "\n";
            std::cout << "Batch: " << seat->student->batchID << "\n";
            std::cout << "Subject: " << seat->student->subject.c_str() << "\n";
            return seat->student;
        }
        else {
            std::cout << "Seat is currently vacant.\n";
            return nullptr;
        }
    }

    // Load from file
    void loadFromFile(const char* fileName) {
        std::ifstream file(fileName);
        if (!file) {
            std::cout << "Cannot open file.\n";
            return;
        }

        int roll, batch;
        char subject[100];
        int count = 0;

        while (file >> batch >> roll >> subject) {
            insertStudent(roll, batch, subject);
            count++;
        }

        file.close();
        std::cout << "Loaded " << count << " students from file.\n";
    }

    // Save to file
    void saveToFile(const char* fileName) {
        std::ofstream file(fileName);
        if (!file) {
            std::cout << "Cannot create file.\n";
            return;
        }

        Block* block = firstBlock;
        while (block) {
            Floor* floor = block->firstFloor;
            while (floor) {
                Room* room = floor->firstRoom;
                while (room) {
                    for (int r = 0; r < gridSize; r++) {
                        for (int c = 0; c < gridSize; c++) {
                            Seat* seat = room->getSeat(r, c);
                            if (seat && seat->student) {
                                file << seat->student->rollNumber << " "
                                    << seat->student->batchID << " "
                                    << seat->student->subject.c_str() << "\n";
                            }
                        }
                    }
                    room = room->next;
                }
                floor = floor->next;
            }
            block = block->next;
        }

        file.close();
        std::cout << "Seating plan saved to " << fileName << "\n";
    }

    // Navigate seating plan interactively
    void navigateSeatingPlan() {
        char currentBlock = 'A';
        int currentFloor = 1;
        int currentRoom = 1;

        std::cout << "\n=== Interactive Navigation ===\n";
        std::cout << "Use: w(up floor) s(down floor) a(prev room) d(next room) q(quit)\n\n";

        while (true) {
            printRoom(currentBlock, currentFloor, currentRoom);

            std::cout << "Current: Block " << currentBlock << ", Floor "
                << currentFloor << ", Room " << currentRoom << "\n";
            std::cout << "Command: ";

            char cmd;
            std::cin >> cmd;

            if (cmd == 'q' || cmd == 'Q') break;

            if (cmd == 'w' || cmd == 'W') {
                if (currentFloor < 3) {
                    currentFloor++;
                }
                else if (currentBlock > 'A') {
                    currentBlock--;
                    currentFloor = 3;
                }
            }
            else if (cmd == 's' || cmd == 'S') {
                if (currentFloor > 1) {
                    currentFloor--;
                }
                else if (currentBlock < 'C') {
                    currentBlock++;
                    currentFloor = 1;
                }
            }
            else if (cmd == 'a' || cmd == 'A') {
                if (currentRoom > 1) {
                    currentRoom--;
                }
                else if (currentFloor > 1) {
                    currentFloor--;
                    currentRoom = 10;
                }
                else if (currentBlock > 'A') {
                    currentBlock--;
                    currentFloor = 3;
                    currentRoom = 10;
                }
            }
            else if (cmd == 'd' || cmd == 'D') {
                if (currentRoom < 10) {
                    currentRoom++;
                }
                else if (currentFloor < 3) {
                    currentFloor++;
                    currentRoom = 1;
                }
                else if (currentBlock < 'C') {
                    currentBlock++;
                    currentFloor = 1;
                    currentRoom = 1;
                }
            }
        }
    }

    // Display summary
    void displaySummary() {
        int totalStudents = 0;
        int totalSeats = 0;
        int totalRooms = 0;
        int totalFloors = 0;

        Block* block = firstBlock;
        while (block) {
            Floor* floor = block->firstFloor;
            while (floor) {
                totalFloors++;
                Room* room = floor->firstRoom;
                while (room) {
                    totalRooms++;
                    totalSeats += gridSize * gridSize;
                    totalStudents += room->occupiedSeats;
                    room = room->next;
                }
                floor = floor->next;
            }
            block = block->next;
        }

        std::cout << "\n=== Seating System Summary ===\n";
        std::cout << "Total Blocks: " << totalBlocks << "\n";
        std::cout << "Total Floors: " << totalFloors << "\n";
        std::cout << "Total Rooms: " << totalRooms << "\n";
        std::cout << "Total Seats: " << totalSeats << "\n";
        std::cout << "Occupied Seats: " << totalStudents << "\n";
        std::cout << "Empty Seats: " << (totalSeats - totalStudents) << "\n";
        std::cout << "Occupancy Rate: "
            << (totalSeats > 0 ? (totalStudents * 100.0 / totalSeats) : 0)
            << "%\n\n";
    }

    // Validate integrity (debug function)
    void validateIntegrity() {
        std::cout << "\n=== Validating System Integrity ===\n";
        int errors = 0;

        Block* block = firstBlock;
        while (block) {
            if (block->next && block->next->prev != block) {
                std::cout << "Error: Block linkage broken!\n";
                errors++;
            }

            Floor* floor = block->firstFloor;
            while (floor) {
                if (floor->next && floor->next->prev != floor) {
                    std::cout << "Error: Floor linkage broken!\n";
                    errors++;
                }

                Room* room = floor->firstRoom;
                while (room) {
                    if (room->next && room->next->prev != room) {
                        std::cout << "Error: Room linkage broken!\n";
                        errors++;
                    }

                    // Check seat connections
                    for (int r = 0; r < gridSize; r++) {
                        for (int c = 0; c < gridSize; c++) {
                            Seat* seat = room->getSeat(r, c);
                            if (!seat) {
                                std::cout << "Error: Missing seat at (" << r << "," << c << ")\n";
                                errors++;
                            }
                        }
                    }

                    room = room->next;
                }
                floor = floor->next;
            }
            block = block->next;
        }

        if (errors == 0) {
            std::cout << "System integrity: OK\n";
        }
        else {
            std::cout << "Total errors found: " << errors << "\n";
        }
        std::cout << "\n";
    }
};

