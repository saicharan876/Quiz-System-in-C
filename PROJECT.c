#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_QUESTIONS 100
#define MAX_LINE_LEN 500

typedef struct {
    char question[MAX_LINE_LEN];
    char options[4][MAX_LINE_LEN];
    char correct_option;
    char user_option;
} Quiz;

void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

int countQuestions(FILE *fp) {
    char ch;
    int lines = 0;
    while ((ch = fgetc(fp)) != EOF) {
        if (ch == '\n') lines++;
    }
    rewind(fp);
    return lines / 5; // 1 question + 4 options
}

void loadQuestions(Quiz *quiz, int total, FILE *fp) {
    for (int i = 0; i < total; i++) {
        fgets(quiz[i].question, MAX_LINE_LEN, fp);
        for (int j = 0; j < 4; j++) {
            fgets(quiz[i].options[j], MAX_LINE_LEN, fp);
        }
    }
}

void loadAnswers(Quiz *quiz, int total, FILE *fp) {
    char buffer[MAX_LINE_LEN];
    for (int i = 0; i < total; i++) {
        fgets(buffer, MAX_LINE_LEN, fp);
        quiz[i].correct_option = buffer[0];
    }
}

void conductQuiz(Quiz *quiz, int total) {
    for (int i = 0; i < total; i++) {
        printf("\nQ%d) %s", i + 1, quiz[i].question);
        for (int j = 0; j < 4; j++) {
            printf("%s", quiz[i].options[j]);
        }

        printf("Your answer (A/B/C/D): ");
        scanf(" %c", &quiz[i].user_option);

        // Flush remaining newline
        while (getchar() != '\n');
    }
}

int calculateScore(Quiz *quiz, int total) {
    int score = 0;
    for (int i = 0; i < total; i++) {
        if (quiz[i].user_option == quiz[i].correct_option) {
            score++;
        }
    }
    return score;
}

void printResults(Quiz *quiz, int total, int roll, char *name, int score) {
    printf("\n-------------------------\n");
    printf("Roll No: %d\nName   : %s\nScore  : %d/%d\n", roll, name, score, total);
    printf("-------------------------\n");

    for (int i = 0; i < total; i++) {
        printf("\nQ%d) %s", i + 1, quiz[i].question);
        printf("Your Answer   : %c\n", quiz[i].user_option);
        printf("Correct Answer: %c\n", quiz[i].correct_option);
        printf("---------------------------------------------------\n");
    }
}

void saveResultToFile(int roll, char *name, int score, int total) {
    FILE *fp = fopen("RESULTS.txt", "a");
    if (!fp) {
        printf("Error writing results.\n");
        return;
    }
    fprintf(fp, "Roll No: %d, Name: %s, Score: %d/%d\n", roll, name, score, total);
    fclose(fp);
}

int main() {
    Quiz quiz[MAX_QUESTIONS];
    char name[50];
    int roll;

    printf("Enter Your Name: ");
    fgets(name, sizeof(name), stdin);
    trim_newline(name);

    printf("Enter Your Roll Number: ");
    scanf("%d", &roll);
    getchar(); // consume newline

    FILE *fq = fopen("QUESTIONS.txt", "r");
    FILE *fa = fopen("ANSWERS.txt", "r");

    if (!fq || !fa) {
        printf("Error: Could not open questions or answers file.\n");
        return 1;
    }

    int total = countQuestions(fq);
    loadQuestions(quiz, total, fq);
    loadAnswers(quiz, total, fa);

    conductQuiz(quiz, total);
    int score = calculateScore(quiz, total);
    printResults(quiz, total, roll, name, score);
    saveResultToFile(roll, name, score, total);

    fclose(fq);
    fclose(fa);
    return 0;
}
