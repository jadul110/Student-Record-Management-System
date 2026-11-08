#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

// IDs
#define ID_NAME       101
#define ID_ROLL       102
#define ID_FATHER     103
#define ID_CASTE      104
#define ID_DISTRICT   105
#define ID_GPA        106

#define ID_ADD        201
#define ID_UPDATE     202
#define ID_DELETE     203
#define ID_NEXT       204
#define ID_EXIT       205

// Student Structure
struct Student
{
    string name, roll, father;
    string caste, district, gpa;
};

vector<Student> students;

int currentIndex = -1;

// GUI Handles
HWND hName, hRoll, hFather;
HWND hCaste, hDistrict, hGPA;

HWND hAdd, hUpdate, hDelete, hNext, hExit;

// Brushes
HBRUSH hBackground;
HBRUSH hEditBackground;

HBRUSH hAddColor;
HBRUSH hUpdateColor;
HBRUSH hDeleteColor;
HBRUSH hNextColor;
HBRUSH hExitColor;

HFONT hFont;

// Get Text
string GetText(HWND hwnd)
{
    char buffer[256];
    GetWindowTextA(hwnd, buffer, 256);
    return string(buffer);
}

// Clear Fields
void ClearFields()
{
    SetWindowTextA(hName, "");
    SetWindowTextA(hRoll, "");
    SetWindowTextA(hFather, "");
    SetWindowTextA(hCaste, "");
    SetWindowTextA(hDistrict, "");
    SetWindowTextA(hGPA, "");
}

// Get Student
Student GetStudent()
{
    Student s;

    s.name = GetText(hName);
    s.roll = GetText(hRoll);
    s.father = GetText(hFather);
    s.caste = GetText(hCaste);
    s.district = GetText(hDistrict);
    s.gpa = GetText(hGPA);

    return s;
}

// Find Student
int FindStudent(string roll)
{
    for(int i = 0; i < students.size(); i++)
    {
        if(students[i].roll == roll)
            return i;
    }

    return -1;
}

// Display Student
void DisplayStudent(int index)
{
    if(index < 0 || index >= students.size())
        return;

    SetWindowTextA(hName, students[index].name.c_str());
    SetWindowTextA(hRoll, students[index].roll.c_str());
    SetWindowTextA(hFather, students[index].father.c_str());
    SetWindowTextA(hCaste, students[index].caste.c_str());
    SetWindowTextA(hDistrict, students[index].district.c_str());
    SetWindowTextA(hGPA, students[index].gpa.c_str());
}

// Load CSV
void LoadCSV()
{
    students.clear();

    ifstream file("students.csv");

    string line;

    while(getline(file, line))
    {
        stringstream ss(line);

        Student s;

        getline(ss, s.name, ',');
        getline(ss, s.roll, ',');
        getline(ss, s.father, ',');
        getline(ss, s.caste, ',');
        getline(ss, s.district, ',');
        getline(ss, s.gpa, ',');

        if(!s.roll.empty())
            students.push_back(s);
    }

    file.close();
}

// Save CSV
bool SaveCSV()
{
    ofstream file("students.csv");

    if(!file.is_open())
        return false;

    for(int i = 0; i < students.size(); i++)
    {
        file << students[i].name << ","
             << students[i].roll << ","
             << students[i].father << ","
             << students[i].caste << ","
             << students[i].district << ","
             << students[i].gpa << endl;
    }

    file.close();

    return true;
}

// Button Drawing
void DrawButton(LPDRAWITEMSTRUCT dis)
{
    HBRUSH brush = NULL;

    switch(dis->CtlID)
    {
        case ID_ADD:
            brush = hAddColor;
            break;

        case ID_UPDATE:
            brush = hUpdateColor;
            break;

        case ID_DELETE:
            brush = hDeleteColor;
            break;

        case ID_NEXT:
            brush = hNextColor;
            break;

        case ID_EXIT:
            brush = hExitColor;
            break;
    }

    if(brush == NULL)
        return;

    FillRect(dis->hDC, &dis->rcItem, brush);

    SetBkMode(dis->hDC, TRANSPARENT);
    SetTextColor(dis->hDC, RGB(255,255,255));

    char text[100];

    GetWindowTextA(dis->hwndItem, text, 100);

    RECT rect = dis->rcItem;

    if(dis->itemState & ODS_SELECTED)
        OffsetRect(&rect, 1, 1);

    DrawTextA(
        dis->hDC,
        text,
        -1,
        &rect,
        DT_CENTER | DT_VCENTER | DT_SINGLELINE
    );

    if(dis->itemState & ODS_FOCUS)
    {
        InflateRect(&rect, -5, -5);
        DrawFocusRect(dis->hDC, &rect);
    }
}

// Window Procedure
LRESULT CALLBACK WindowProcedure(
    HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch(msg)
    {
        case WM_CREATE:
        {
            // Background
            hBackground = CreateSolidBrush(
                RGB(220,235,250));

            hEditBackground = CreateSolidBrush(
                RGB(255,255,255));

            // Button Colors
            hAddColor = CreateSolidBrush(
                RGB(39,174,96));

            hUpdateColor = CreateSolidBrush(
                RGB(41,128,185));

            hDeleteColor = CreateSolidBrush(
                RGB(220,53,69));

            hNextColor = CreateSolidBrush(
                RGB(125,60,180));

            hExitColor = CreateSolidBrush(
                RGB(70,70,70));

            // Font
            hFont = CreateFontA(
                20,0,0,0,FW_NORMAL,
                FALSE,FALSE,FALSE,
                DEFAULT_CHARSET,
                OUT_DEFAULT_PRECIS,
                CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY,
                DEFAULT_PITCH,
                "Segoe UI"
            );

            // Heading
            HWND heading = CreateWindowA(
                "STATIC",
                "STUDENT RECORD MANAGEMENT SYSTEM",
                WS_VISIBLE | WS_CHILD | SS_CENTER,
                60,25,650,45,
                hwnd,NULL,NULL,NULL
            );

            SendMessageA(
                heading,WM_SETFONT,
                (WPARAM)hFont,TRUE
            );

            // Labels
            const char* labels[] =
            {
                "Student Name:",
                "Roll Number:",
                "Father's Name:",
                "Caste:",
                "District:",
                "GPA:"
            };

            int y = 100;

            for(int i=0;i<6;i++)
            {
                HWND label = CreateWindowA(
                    "STATIC",
                    labels[i],
                    WS_VISIBLE | WS_CHILD,
                    130,y,140,30,
                    hwnd,NULL,NULL,NULL
                );

                SendMessageA(
                    label,WM_SETFONT,
                    (WPARAM)hFont,TRUE
                );

                y += 55;
            }

            // Input Fields
            hName = CreateWindowA(
                "EDIT","",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                290,100,280,32,
                hwnd,(HMENU)ID_NAME,NULL,NULL
            );

            hRoll = CreateWindowA(
                "EDIT","",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                290,155,280,32,
                hwnd,(HMENU)ID_ROLL,NULL,NULL
            );

            hFather = CreateWindowA(
                "EDIT","",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                290,210,280,32,
                hwnd,(HMENU)ID_FATHER,NULL,NULL
            );

            hCaste = CreateWindowA(
                "EDIT","",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                290,265,280,32,
                hwnd,(HMENU)ID_CASTE,NULL,NULL
            );

            hDistrict = CreateWindowA(
                "EDIT","",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                290,320,280,32,
                hwnd,(HMENU)ID_DISTRICT,NULL,NULL
            );

            hGPA = CreateWindowA(
                "EDIT","",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                290,375,280,32,
                hwnd,(HMENU)ID_GPA,NULL,NULL
            );

            // Assign Font
            HWND fields[] =
            {
                hName,hRoll,hFather,
                hCaste,hDistrict,hGPA
            };

            for(int i=0;i<6;i++)
            {
                SendMessageA(
                    fields[i],WM_SETFONT,
                    (WPARAM)hFont,TRUE
                );
            }

            // ADD
            hAdd = CreateWindowA(
                "BUTTON","ADD",
                WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                100,450,115,45,
                hwnd,(HMENU)ID_ADD,NULL,NULL
            );

            // UPDATE
            hUpdate = CreateWindowA(
                "BUTTON","UPDATE",
                WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                230,450,115,45,
                hwnd,(HMENU)ID_UPDATE,NULL,NULL
            );

            // DELETE
            hDelete = CreateWindowA(
                "BUTTON","DELETE",
                WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                360,450,115,45,
                hwnd,(HMENU)ID_DELETE,NULL,NULL
            );

            // NEXT
            hNext = CreateWindowA(
                "BUTTON","NEXT",
                WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                490,450,115,45,
                hwnd,(HMENU)ID_NEXT,NULL,NULL
            );

            // EXIT
            hExit = CreateWindowA(
                "BUTTON","EXIT",
                WS_VISIBLE | WS_CHILD | BS_OWNERDRAW,
                300,520,120,45,
                hwnd,(HMENU)ID_EXIT,NULL,NULL
            );

            // Initial States
            EnableWindow(hUpdate,FALSE);
            EnableWindow(hDelete,FALSE);

            LoadCSV();

            break;
        }

        // Static Text Color
        case WM_CTLCOLORSTATIC:
        {
            HDC hdc = (HDC)wp;

            SetBkMode(hdc,TRANSPARENT);
            SetTextColor(hdc,RGB(25,45,80));

            return (LRESULT)hBackground;
        }

        // Input Field Color
        case WM_CTLCOLOREDIT:
        {
            HDC hdc = (HDC)wp;

            SetBkColor(hdc,RGB(255,255,255));

            return (LRESULT)hEditBackground;
        }

        // Colored Buttons
        case WM_DRAWITEM:
        {
            DrawButton(
                (LPDRAWITEMSTRUCT)lp
            );

            return TRUE;
        }

        // Button Actions
        case WM_COMMAND:
        {
            switch(LOWORD(wp))
            {
                // ADD
                case ID_ADD:
                {
                    Student s = GetStudent();

                    if(s.name.empty() || s.roll.empty())
                    {
                        MessageBoxA(
                            hwnd,
                            "Enter Name and Roll Number!",
                            "Warning",
                            MB_OK | MB_ICONWARNING
                        );

                        break;
                    }

                    if(FindStudent(s.roll)!=-1)
                    {
                        MessageBoxA(
                            hwnd,
                            "Roll Number already exists!",
                            "Error",
                            MB_OK | MB_ICONERROR
                        );

                        break;
                    }

                    students.push_back(s);

                    if(!SaveCSV())
                    {
                        students.pop_back();

                        MessageBoxA(
                            hwnd,
                            "Unable to save CSV file!",
                            "File Error",
                            MB_OK | MB_ICONERROR
                        );

                        break;
                    }

                    ClearFields();

                    MessageBoxA(
                        hwnd,
                        "Record Added Successfully!",
                        "Success",
                        MB_OK
                    );

                    break;
                }

                // NEXT
                case ID_NEXT:
                {
                    if(students.empty())
                    {
                        MessageBoxA(
                            hwnd,
                            "No student records found!",
                            "Information",
                            MB_OK
                        );

                        break;
                    }

                    EnableWindow(hAdd,FALSE);
                    EnableWindow(hUpdate,TRUE);
                    EnableWindow(hDelete,TRUE);

                    currentIndex++;

                    if(currentIndex >= students.size())
                        currentIndex=0;

                    DisplayStudent(currentIndex);

                    break;
                }

                // UPDATE
                case ID_UPDATE:
                {
                    if(currentIndex<0 ||
                       currentIndex>=students.size())
                        break;

                    Student s=GetStudent();

                    if(s.name.empty() || s.roll.empty())
                        break;

                    int found=FindStudent(s.roll);

                    if(found!=-1 && found!=currentIndex)
                    {
                        MessageBoxA(
                            hwnd,
                            "Roll Number already exists!",
                            "Error",
                            MB_OK
                        );

                        break;
                    }

                    Student old = students[currentIndex];

                    students[currentIndex]=s;

                    if(!SaveCSV())
                    {
                        students[currentIndex]=old;

                        MessageBoxA(
                            hwnd,
                            "Unable to save changes!",
                            "File Error",
                            MB_OK
                        );

                        break;
                    }

                    MessageBoxA(
                        hwnd,
                        "Record Updated Successfully!",
                        "Success",
                        MB_OK
                    );

                    break;
                }

                // DELETE
                case ID_DELETE:
                {
                    if(currentIndex<0 ||
                       currentIndex>=students.size())
                        break;

                    int answer=MessageBoxA(
                        hwnd,
                        "Delete this student record?",
                        "Confirm Delete",
                        MB_YESNO | MB_ICONQUESTION
                    );

                    if(answer==IDYES)
                    {
                        Student deleted=students[currentIndex];

                        students.erase(
                            students.begin()+currentIndex
                        );

                        if(!SaveCSV())
                        {
                            students.insert(
                                students.begin()+currentIndex,
                                deleted
                            );

                            MessageBoxA(
                                hwnd,
                                "Unable to save changes!",
                                "File Error",
                                MB_OK
                            );

                            break;
                        }

                        if(students.empty())
                        {
                            currentIndex=-1;

                            ClearFields();

                            EnableWindow(hAdd,TRUE);
                            EnableWindow(hUpdate,FALSE);
                            EnableWindow(hDelete,FALSE);
                        }
                        else
                        {
                            if(currentIndex>=students.size())
                                currentIndex=0;

                            DisplayStudent(currentIndex);
                        }

                        MessageBoxA(
                            hwnd,
                            "Record Deleted Successfully!",
                            "Success",
                            MB_OK
                        );
                    }

                    break;
                }

                // EXIT
                case ID_EXIT:
                {
                    DestroyWindow(hwnd);
                    break;
                }
            }

            break;
        }

        case WM_DESTROY:
        {
            DeleteObject(hBackground);
            DeleteObject(hEditBackground);

            DeleteObject(hAddColor);
            DeleteObject(hUpdateColor);
            DeleteObject(hDeleteColor);
            DeleteObject(hNextColor);
            DeleteObject(hExitColor);

            DeleteObject(hFont);

            PostQuitMessage(0);

            break;
        }

        default:
            return DefWindowProcA(
                hwnd,msg,wp,lp
            );
    }

    return 0;
}

// Main Function
int WINAPI WinMain(
    HINSTANCE hInst,
    HINSTANCE hPrev,
    LPSTR args,
    int ncmdshow)
{
    WNDCLASSA wc={};

    wc.hbrBackground =
        CreateSolidBrush(RGB(220,235,250));

    wc.hCursor=LoadCursor(NULL,IDC_ARROW);

    wc.hInstance=hInst;

    wc.lpszClassName="StudentSystem";

    wc.lpfnWndProc=WindowProcedure;

    if(!RegisterClassA(&wc))
        return 0;

    HWND hwnd=CreateWindowA(
        "StudentSystem",
        "Student Record Management System",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        200,50,800,650,
        NULL,NULL,hInst,NULL
    );

    if(!hwnd)
        return 0;

    ShowWindow(hwnd,ncmdshow);
    UpdateWindow(hwnd);

    MSG msg={};

    while(GetMessageA(&msg,NULL,0,0)>0)
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    DeleteObject(wc.hbrBackground);

    return 0;
}
