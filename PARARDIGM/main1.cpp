#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stack>
#include <windows.h> 
#define MAX_TEXT_SIZE 1024

class DecrtptEcrypt {
private:
    HINSTANCE hInst;
    typedef void (*EncryptFunction)(const char*, int, char*);
    typedef void (*DecryptFunction)(const char*, int, char*);
    EncryptFunction encrypt;
    DecryptFunction decrypt;

public:
    DecrtptEcrypt() {
        hInst = LoadLibrary(TEXT("D:\\KSE\\paradigm\\HW3PP\\Dll1\\x64\\Debug\\Dll1.dll"));
        if (hInst == NULL) {
            std::cerr << "Lib not found" << std::endl;
            exit(-1); 
        }

        typedef void (*EncryptFunction)(const char*, int, char*);
        typedef void (*DecryptFunction)(const char*, int, char*);

        EncryptFunction encrypt = (EncryptFunction)GetProcAddress(hInst, "ecrypt");
        DecryptFunction decrypt = (DecryptFunction)GetProcAddress(hInst, "decrypt");

        if (encrypt == NULL || decrypt == NULL) {
            std::cerr << "Function not found" << std::endl;
            FreeLibrary(hInst);
            exit(-1); 
        }
    }

    ~DecrtptEcrypt() {
        FreeLibrary(hInst);
    }

    void execute() {
        std::string inputText;
        std::cout << "Enter text to encrypt: ";
        std::getline(std::cin, inputText);

        int key;
        std::cout << "Enter encryption key: ";
        std::cin >> key;

        char* encryptedText = new char[inputText.length() + 1];
        encrypt(inputText.c_str(), key, encryptedText);
        std::cout << "Encrypted Text: " << encryptedText << std::endl;

        char* decryptedText = new char[inputText.length() + 1];
        decrypt(encryptedText, key, decryptedText);
        std::cout << "Decrypted Text: " << decryptedText << std::endl;

        delete[] encryptedText;
        delete[] decryptedText;
    }
};

class FileSveload {
private:
    char* text;

public:
    FileSveload(char* externalText) : text(externalText) {}

    void save_to_file() {
        char filename[100];
        printf("Enter the file name for saving: ");
        fgets(filename, sizeof(filename), stdin);
        if (filename[strlen(filename) - 1] == '\n') {
            filename[strlen(filename) - 1] = '\0';
        }

        FILE* file;
        errno_t err = fopen_s(&file, filename, "w");
        if (err != 0) {
            perror("Error opening file");
            return;
        }

        fputs(text, file);
        fclose(file);
        printf("Text has been saved successfully\n");
    }

    void load_from_file() {
        char filename[100];
        printf("Enter the file name for loading: ");
        fgets(filename, sizeof(filename), stdin);
        if (filename[strlen(filename) - 1] == '\n') {
            filename[strlen(filename) - 1] = '\0';
        }

        FILE* file;
        errno_t err = fopen_s(&file, filename, "r");
        if (err != 0) {
            perror("Error opening file");
            return;
        }

        char ch;
        int index = 0;
        while ((ch = fgetc(file)) != EOF && index < MAX_TEXT_SIZE - 1) {
            text[index++] = ch;
        }
        text[index] = '\0';

        fclose(file);
        printf("Text has been loaded successfully\n");
    }
};

class ClipboardManager {
private:
    char clipboard[MAX_TEXT_SIZE] = "";
    

public:
    void copy(const char* source, int start, int length) {
        if (start + length > strlen(source)) {
            length = strlen(source) - start;
        }

        strncpy_s(clipboard, sizeof(clipboard), source + start, length);
        clipboard[length] = '\0';
    }

    const char* getClipboardContent() const {
        return clipboard;
    }
};

class TextEditor {
private:
    char text[MAX_TEXT_SIZE] = "";
    DecrtptEcrypt decrtptEcrypt;
    FileSveload fileHandler;
    ClipboardManager clipboardManager;
    std::stack<std::string> undoStack;
    std::stack<std::string> redoStack;
    int cursorPosition = 0;

    void pushToUndo() {
        undoStack.push(text);
        while (!redoStack.empty()) {
            redoStack.pop();
        }
    }

public:
    TextEditor() : fileHandler(text) {}

    void append_text() {
        char input[100];
        printf("Enter text to append: ");
        fgets(input, sizeof(input), stdin);

        size_t len = strlen(text);
        strncat_s(text, sizeof(text), input, sizeof(text) - len - 1);

        if (len + strlen(input) < sizeof(text) && text[len + strlen(input) - 1] == '\n') {
            text[len + strlen(input) - 1] = '\0';
        }
    }

    void start_new_line() {
        strncat_s(text, sizeof(text), "\n", sizeof(text) - strlen(text) - 1);
        printf("New line is started\n");
    }

    void print_text() {
        printf("%s\n", text);
    }

    void insert_text_by_index() {
        int line, index;
        char input[100];

        printf("Select a row and position (starting from 0): ");
        scanf_s("%d %d", &line, &index);
        while (getchar() != '\n');

        printf("Enter the text to insert: ");
        fgets(input, sizeof(input), stdin);

        char* lines[MAX_TEXT_SIZE];
        int lineCount = 0;
        char* context = NULL;
        char* token = strtok_s(text, "\n", &context);
        while (token) {
            lines[lineCount++] = token;
            token = strtok_s(NULL, "\n", &context);
        }

        if (line >= lineCount) {
            printf("Invalid line number!\n");
            return;
        }

        char temp[MAX_TEXT_SIZE];
        strcpy_s(temp, sizeof(temp), lines[line] + index);
        strcpy_s(lines[line] + index, sizeof(lines[line]) - index, input);
        strcat_s(lines[line], sizeof(lines[line]), temp);

        int pos = 0;
        for (int i = 0; i < lineCount; i++) {
            strcpy_s(text + pos, sizeof(text) - pos, lines[i]);
            pos += strlen(lines[i]);
            text[pos++] = '\n';
        }
        text[pos] = '\0';
    }

    void delete_text_by_index() {
        int line, index, length;
        printf("Select a row, position (starting from 0) and length for deletion: ");
        scanf_s("%d %d %d", &line, &index, &length);
        while (getchar() != '\n');


        char* lines[MAX_TEXT_SIZE];
        int lineCount = 0;
        char* context = NULL;
        char* token = strtok_s(text, "\n", &context);
        while (token) {
            lines[lineCount++] = token;
            token = strtok_s(NULL, "\n", &context);
        }

        if (line >= lineCount) {
            printf("Invalid line number!\n");
            return;
        }

        size_t lineLength = strlen(lines[line]);
        if (index + length > lineLength) {
            length = lineLength - index;

            if (index >= lineLength || length <= 0) {
                printf("Invalid position or length!\n");
                return;
            }


            memmove(lines[line] + index, lines[line] + index + length, lineLength - index - length + 1);


            int pos = 0;
            for (int i = 0; i < lineCount; i++) {
                strcpy_s(text + pos, sizeof(text) - pos, lines[i]);
                pos += strlen(lines[i]);
                text[pos++] = '\n';
            }
            text[pos] = '\0';

            printf("Text has been deleted successfully\n");
        }
    }



    void cut(int start, int length) {
        pushToUndo();

        if (start + length > strlen(text)) {
            length = strlen(text) - start;
        }

        clipboardManager.copy(text, start, length);
        strcpy_s(text + start, sizeof(text) - start, text + start + length);
    }

    void copy(int start, int length) {
        clipboardManager.copy(text, start, length);
    }

    void paste(int position) {
        pushToUndo();

        char temp[MAX_TEXT_SIZE];
        strcpy_s(temp, sizeof(temp), text + position);
        strcpy_s(text + position, sizeof(text) - position, clipboardManager.getClipboardContent());
        strcat_s(text, sizeof(text), temp);
    }

    void undo() {
        if (!undoStack.empty()) {
            redoStack.push(text);
            strcpy_s(text, sizeof(text), undoStack.top().c_str());
            undoStack.pop();
        }
        else {
            printf("Nothing to undo!\n");
        }
    }

    void redo() {
        if (!redoStack.empty()) {
            undoStack.push(text);
            strcpy_s(text, sizeof(text), redoStack.top().c_str());
            redoStack.pop();
        }
        else {
            printf("Nothing to redo!\n");
        }
    }

    void insertWithReplacement(int position, const char* newText) {
        pushToUndo();

        if (position > strlen(text)) {
            position = strlen(text);
        }

        char temp[MAX_TEXT_SIZE];
        strcpy_s(temp, sizeof(temp), text + position);
        strcpy_s(text + position, sizeof(text) - position, newText);
        strcat_s(text, sizeof(text), temp);
    }
    void moveCursorLeft() {
        if (cursorPosition > 0) {
            cursorPosition--;
        }
    }

    void moveCursorRight() {
        if (cursorPosition < strlen(text)) {
            cursorPosition++;
        }
    }

    void moveCursorToStart() {
        cursorPosition = 0;
    }

    void moveCursorToEnd() {
        cursorPosition = strlen(text);
    }

    void insertAtCursor(const char* newText) {
        pushToUndo();
        char temp[MAX_TEXT_SIZE];
        strcpy_s(temp, sizeof(temp), text + cursorPosition);
        strcpy_s(text + cursorPosition, sizeof(text) - cursorPosition, newText);
        strcat_s(text, sizeof(text), temp);
        cursorPosition += strlen(newText);
    }

    void deleteAtCursor(int num_symbols) {
        pushToUndo();
        if (cursorPosition + num_symbols > strlen(text)) {
            num_symbols = strlen(text) - cursorPosition;
        }
        strcpy_s(text + cursorPosition, sizeof(text) - cursorPosition, text + cursorPosition + num_symbols);
    }


    void menu() {
        while (1) {
            printf("Choose the command:\n");
            printf(" 1. Append text symbols to the end\n");
            printf(" 2. Start the new line\n");
            printf(" 3. Use files to loading/saving the information\n");
            printf(" 4. Print the current text to console\n");
            printf(" 5. Insert the text by line and symbol index\n");
            printf(" 6. Exit\n");
            printf(" 7. Clear terminal.\n");
            printf(" 8. Delete text\n");
            printf(" 9. Cut\n");
            printf("10. Copy\n");
            printf("11. Paste\n");
            printf("12. Undo\n");
            printf("13. Redo\n");
            printf("14. Insert with replacement\n");
            printf("15. Move cursor left\n");
            printf("16. Move cursor right\n");
            printf("17. Move cursor to start\n");
            printf("18. Move cursor to end\n");
            printf("19. Insert text at cursor\n");
            printf("20. Delete text at cursor\n");
            printf("21. Decrtpt/Ecrypt\n");

            int choice;
            scanf_s("%d", &choice);
            while (getchar() != '\n');

            switch (choice) {
            case 1:
                append_text();
                break;
            case 2:
                start_new_line();
                break;
            case 3:
                printf("Choose:\n");
                printf("1. Save to file\n");
                printf("2. Load from file\n");
                int file_choice;
                scanf_s("%d", &file_choice);
                while (getchar() != '\n');
                if (file_choice == 1) {
                    fileHandler.save_to_file();
                }
                else if (file_choice == 2) {
                    fileHandler.load_from_file();
                }
                else {
                    printf("Invalid choice!\n");
                }
                break;
            case 4:
                print_text();
                break;
            case 5:
                insert_text_by_index();
                break;
            case 6:
                exit(0);
            case 7:
                system("cls");
                break;
            case 8:
                delete_text_by_index();
                break;
            case 9:
                int start, length;
                printf("Enter start position and length to cut: ");
                scanf_s("%d %d", &start, &length);
                cut(start, length);
                break;
            case 10:
                printf("Enter start position and length to copy: ");
                scanf_s("%d %d", &start, &length);
                copy(start, length);
                break;
            case 11:
                printf("Enter position to paste: ");
                scanf_s("%d", &start);
                paste(start);
                break;
            case 12:
                undo();
                break;
            case 13:
                redo();
                break;
            case 14:
                printf("Enter position to insert: ");
                scanf_s("%d", &start);
                while (getchar() != '\n');
                char replacement[MAX_TEXT_SIZE];
                printf("Enter text to insert: ");
                fgets(replacement, MAX_TEXT_SIZE, stdin);
                if (replacement[strlen(replacement) - 1] == '\n') {
                    replacement[strlen(replacement) - 1] = '\0';
                }
                insertWithReplacement(start, replacement);
                break;
            case 15:
                moveCursorLeft();
                break;
            case 16:
                moveCursorRight();
                break;
            case 17:
                moveCursorToStart();
                break;
            case 18:
                moveCursorToEnd();
                break;
            case 19:
                char input[100];
                printf("Enter the text to insert at cursor: ");
                fgets(input, sizeof(input), stdin);
                insertAtCursor(input);
                break;
            case 20:
                int num_symbols;
                printf("Enter number of characters to delete at cursor: ");
                scanf_s("%d", &num_symbols);
                deleteAtCursor(num_symbols);
            case 21:
                decrtptEcrypt.execute(); 
                break;
            default:
                printf("Invalid choice!\n");
            }
        }
    }
};

int main() {
    TextEditor editor;
    editor.menu();
    return 0;
}