#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAX_SENTENCE_LENGTH 100

char* generate_sentence() {
    char* sentences[] = {"Hello My name is Minjunho.", "Professor is so handsome.", "Linux is too hard to learn."};
    int num_sentences = sizeof(sentences) / sizeof(sentences[0]);
    int rand_index = rand() % num_sentences;

    char* sentence = malloc(strlen(sentences[rand_index]) + 1);
     if (sentence == NULL) {
         fprintf(stderr, "Memory allocation error\n");
         exit(EXIT_FAILURE);
     }

    strcpy(sentence, sentences[rand_index]);
    return sentence;
}

void calculate_typing_speed() {
    char typed_sentence[MAX_SENTENCE_LENGTH];
    char* sentence = generate_sentence();

    printf("Type the following sentence:\n   %s\n", sentence);

    time_t start_time = time(NULL);

    fgets(typed_sentence, MAX_SENTENCE_LENGTH, stdin);
    typed_sentence[strcspn(typed_sentence, "\n")] = '\0'; // Remove trailing newline character

    time_t end_time = time(NULL);
    int elapsed_time = (int)(end_time - start_time);

    int words_typed = 0;
    int accuracy = 0;
    int typo_count = 0;

    // Tokenize the sentence 
    char* word = strtok(sentence, " ");
    while (word != NULL) {
        words_typed++;

        if (strstr(typed_sentence, word) != NULL) {
            accuracy++;
        } else {
            typo_count++;
        }

        word = strtok(NULL, " ");
    }

    printf("\n틀린 횟수: %d\n", typo_count);
    printf("분당 타자수: %d\n", (int)(words_typed * 60 / elapsed_time));
    printf("총 걸린 시간: %d 초\n", elapsed_time);

    free(sentence);
}

int main() {
    srand((unsigned int)time(NULL));
    calculate_typing_speed();

    return 0;
}

