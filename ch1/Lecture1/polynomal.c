#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>


typedef struct polyNode* polyPointer;  // 재정의 뭐라뭐라 오류떠서 그냥 맨 위로 올림
typedef struct polyNode {
	float coef;
	int exp;
	polyPointer link;
} polyNode;

void attach2(float coefficient, int exponent);
void attach3(float coef2, int exp2, polyPointer* ptr);

// **ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ 1번 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ**

#define MAX_DEGREE 101

typedef struct {
	int degree;
	float coef[MAX_DEGREE];
} polynomial;

polynomial Zero() {  // 다항식 하나 0으로 초기화
	polynomial p;
	p.degree = 0;
	for (int i = 0; i < MAX_DEGREE; i++) {
		p.coef[i] = 0;
	}
	return p;
}

bool IsZero(polynomial p) {  // 다항식이 0인지 아닌지 확인
	for (int i = MAX_DEGREE - 1; i >= 0; i--) {
		if (p.coef[i] != 0) return false;
	}
	return true;
}

int COMPARE(int a, int b) {  // 걍 비교
	if (a > b) return 1;
	else if (a < b) return -1;
	else return 0;
}

int Lead_Exp(polynomial p) {  // 맨 뒤에 있는 차수
	for (int i = MAX_DEGREE - 1; i >= 0; i--) {
		if (p.coef[i] != 0) return i;
	}
	return 0;
}

float Coef(polynomial p, int a) {  // 지수 반환
	return p.coef[a];
}

polynomial attach1(polynomial p, float a, int b) {  // 1번째 다항식 함수 더하기
	p.coef[b] += a;
	if (b > p.degree) p.degree = b;
	return p;
}

polynomial Remove(polynomial p, int a) {  // 다항식 지수 하나 없애기
	p.coef[a] = 0;
	if (a == p.degree) { // a가 최고 차수라면
		for (int i = a; i >= 0; i--) {
			if (p.coef[i] != 0) { // 2번째로 큰 차수가 이제 최고 차수
				p.degree = i;
				return p;
			}
		}
	}
	p.degree = 0; // 여기까지 왔다면 p는 아무것도 남아있지 않음
	return p;
}

polynomial Padd(polynomial a, polynomial b) {
	polynomial d = Zero();
	float sum = 0.0f;

	while (!IsZero(a) && !IsZero(b)) {
		switch (COMPARE(Lead_Exp(a), Lead_Exp(b))) {
		case -1:
			d = attach1(d, Coef(b, Lead_Exp(b)), Lead_Exp(b));
			b = Remove(b, Lead_Exp(b));
			break;
		case 0:
			sum = Coef(a, Lead_Exp(a)) + Coef(b, Lead_Exp(b));
			if (sum) d = attach1(d, sum, Lead_Exp(a)); // 서로의 합이 0이 아니라면
			a = Remove(a, Lead_Exp(a));
			b = Remove(b, Lead_Exp(b));
			break;
		case 1:
			d = attach1(d, Coef(a, Lead_Exp(a)), Lead_Exp(a));
			a = Remove(a, Lead_Exp(a));
			break;
		}
	}

	while (!IsZero(a)) {
		d = attach1(d, Coef(a, Lead_Exp(a)), Lead_Exp(a));
		a = Remove(a, Lead_Exp(a));
	}

	while (!IsZero(b)) {
		d = attach1(d, Coef(b, Lead_Exp(b)), Lead_Exp(b));
		b = Remove(b, Lead_Exp(b));
	}

	return d;
}

void PrintPolynomial(FILE* f, polynomial p) {
	int first = 0; // bool 대신 0과 1로..
	for (int i = MAX_DEGREE - 1; i >= 0; i--) {
		if (p.coef[i] != 0) {
			if (first != 0)
				fprintf(f, " + ");
			fprintf(f, "%.0fx^%d", p.coef[i], i);
			first = 1;
		}
	}
	if (first == 0) fprintf(f, "0");
	fprintf(f, "\n");
}

// **ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ 2번 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ**
#define MAX_TERMS 100

typedef struct {
	float coef; // 계수
	int exp; // 지수
} polynomial2;

polynomial2 terms[MAX_TERMS]; // 전역 변수 
int avail = 0;

void padd2(int starta, int finisha, int startb, int finishb, int* startd, int* finishd)
{
	float sum;
	*startd = avail;

	while (starta <= finisha && startb <= finishb) {
		switch (COMPARE(terms[starta].exp, terms[startb].exp)) {
		case -1:
			attach2(terms[startb].coef, terms[startb].exp);
			startb++;
			break;
		case 0:
			sum = terms[starta].coef + terms[startb].coef;
			if (sum)
				attach2(sum, terms[starta].exp);
			starta++;
			startb++;
			break;
		case 1:
			attach2(terms[starta].coef, terms[starta].exp);
			starta++;
			break;
		}
	}

	while (starta <= finisha) {
		attach2(terms[starta].coef, terms[starta].exp);
		starta++;
	}

	while (startb <= finishb) {
		attach2(terms[startb].coef, terms[startb].exp);
		startb++;
	}

	*finishd = avail - 1;
}
void attach2(float coefficient, int exponent)  // 다항식 더하기
{
	if (avail >= MAX_TERMS) {
		fprintf(stderr, "다항식에 항이 너무 많다.");
		exit(1);
	}
	terms[avail].coef = coefficient;
	terms[avail].exp = exponent;
	avail++;
}

void PrintPolynomial2(FILE* f, int start, int finish) {
	for (int i = start; i <= finish - 1; i++) {
		for (int j = i + 1; j <= finish; j++) {
			if (terms[i].exp < terms[j].exp) {
				polynomial2 temp = terms[i];
				terms[i] = terms[j];
				terms[j] = temp;
			}
		}
	}

	int first = 0;
	for (int i = start; i <= finish; i++) {
		if (terms[i].coef != 0) {
			if (first != 0)
				fprintf(f, " + ");
			fprintf(f, "%.0fx^%d", terms[i].coef, terms[i].exp);
			first = 1;
		}
	}
	if (first == 0) fprintf(f, "0");
	fprintf(f, "\n");
}

// **ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ 3번 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ**

void attach3(float coefficient, int exponent, polyPointer* ptr) {  // 3번째 다항식 더하기
	polyPointer temp;
	temp = (polyPointer)malloc(sizeof(struct polyNode));
	temp->coef = coefficient;
	temp->exp = exponent;
	temp->link = NULL;
	(*ptr)->link = temp;
	*ptr = temp;
}

polyPointer padd(polyPointer a, polyPointer b) {
	polyPointer c, rear, temp;
	int sum;

	rear = (polyPointer)malloc(sizeof(struct polyNode));
	rear->link = NULL;
	c = rear;

	while (a && b) {
		switch (COMPARE(a->exp, b->exp)) {
		case -1:
			attach3(b->coef, b->exp, &rear);
			b = b->link;
			break;
		case 0:
			sum = a->coef + b->coef;
			if (sum) attach3(sum, a->exp, &rear);
			a = a->link;
			b = b->link;
			break;
		case 1:
			attach3(a->coef, a->exp, &rear);
			a = a->link;
		}
	}

	while (a) {
		attach3(a->coef, a->exp, &rear);
		a = a->link;
	}
	while (b) {
		attach3(b->coef, b->exp, &rear);
		b = b->link;
	}

	rear->link = NULL;

	temp = c;
	c = c->link;
	free(temp);

	return c;
}
void SortPolynomial(polyPointer head) {  // exp 기준 내림차순으로 정렬
	if (head == NULL) return;
	int count = 0;  // 무한 루프 방지용

	for (polyPointer i = head; i->link != NULL; i = i->link) {
		for (polyPointer j = i->link; j != NULL; j = j->link) {
			if (i->exp < j->exp) {
				float temp_coef = i->coef;
				int temp_exp = i->exp;

				i->coef = j->coef;
				i->exp = j->exp;

				j->coef = temp_coef;
				j->exp = temp_exp;
			}
			count++;
		}
	}
	if (count >= 1000) {
		printf("무한 루프 감지됨: SortPolynomial 중단\n");
	}
}

void PrintPolynomial3(FILE* f, polyPointer p) {
	int first = 0;
	for (polyPointer cur = p; cur != NULL; cur = cur->link) {
		if (cur->coef != 0) {
			if (first != 0) fprintf(f, " + ");
			fprintf(f, "%.0fx^%d", cur->coef, cur->exp);
			first = 1;
		}
	}
	if (first == 0) fprintf(f, "0");
	fprintf(f, "\n");
}

// ** ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ[ 함수 ]ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ**


void func1(FILE* fin, FILE* fout) {

	int x1, x2;
	fscanf(fin, "%d%d", &x1, &x2);

	polynomial a = Zero();
	polynomial b = Zero();

	for (int i = 0; i < x1; i++) {
		float coef;
		int exp;
		fscanf(fin, "%f%d", &coef, &exp);
		a = attach1(a, coef, exp);
	}

	for (int i = 0; i < x2; i++) {
		float coef; int exp;
		fscanf(fin, "%f%d", &coef, &exp);
		b = attach1(b, coef, exp);
	}

	polynomial result1 = Padd(a, b);

	PrintPolynomial(fout, a);
	PrintPolynomial(fout, b);
	PrintPolynomial(fout, result1);

}

void func2(FILE* fin, FILE* fout) {

	int x1, x2;
	fscanf(fin, "%d%d", &x1, &x2);

	avail = 0; // 전역 배열 초기화
	int starta = avail;

	// 다항식 A 입력
	for (int i = 0; i < x1; i++) {
		float coef;
		int exp;
		fscanf(fin, "%f%d", &coef, &exp);
		attach2(coef, exp);
	}
	int finisha = avail - 1;

	// 다항식 B 입력
	int startb = avail;
	for (int i = 0; i < x2; i++) {
		float coef;
		int exp;
		fscanf(fin, "%f%d", &coef, &exp);
		attach2(coef, exp);
	}
	int finishb = avail - 1;

	// padd2 실행
	int startd, finishd;
	padd2(starta, finisha, startb, finishb, &startd, &finishd);

	PrintPolynomial2(fout, starta, finisha);
	PrintPolynomial2(fout, startb, finishb);
	PrintPolynomial2(fout, startd, finishd);

	return;
}

void func3(FILE* fin, FILE* fout) {
	int x1, x2;
	fscanf(fin, "%d%d", &x1, &x2); // 각 다항식의 항 개수

	// A 다항식 입력
	polyPointer a = NULL, a_tail = NULL;
	for (int i = 0; i < x1; i++) {
		float coef;
		int exp;
		fscanf(fin, "%f%d", &coef, &exp);
		polyPointer newNode = (polyPointer)malloc(sizeof(struct polyNode));
		newNode->coef = coef;
		newNode->exp = exp;
		newNode->link = NULL;

		if (a == NULL) {
			a = a_tail = newNode;
		}
		else {
			a_tail->link = newNode;
			a_tail = newNode;
		}
	}

	// B 다항식 입력
	polyPointer b = NULL, b_tail = NULL;
	for (int i = 0; i < x2; i++) {
		float coef;
		int exp;
		fscanf(fin, "%f%d", &coef, &exp);
		polyPointer newNode = (polyPointer)malloc(sizeof(struct polyNode));
		newNode->coef = coef;
		newNode->exp = exp;
		newNode->link = NULL;

		if (b == NULL) {
			b = b_tail = newNode;
		}
		else {
			b_tail->link = newNode;
			b_tail = newNode;
		}
	}

	// 덧셈 수행
	polyPointer c = padd(a, b);
	SortPolynomial(a);
	SortPolynomial(b);
	SortPolynomial(c);
	// 출력

	PrintPolynomial3(fout, a);
	PrintPolynomial3(fout, b);
	PrintPolynomial3(fout, c);

	return;
}

// ** ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ[ Main ]ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ**


int main() {

	clock_t start, end;
	double s1, s2, s3;

	FILE* fin = fopen("Input.txt", "r");
	FILE* fout = fopen("output.txt", "w");

	if (!fin || !fout) {
		printf("파일 열기 실패\n");
		return 0;
	}

	start = clock();
	func1(fin, fout);

	end = clock();
	s1 = ((double)(end - start) * 1000) / CLOCKS_PER_SEC;

	rewind(fin);  // 읽는 포인터 위치 초기화

	start = clock();
	func2(fin, fout);

	end = clock();
	s2 = ((double)(end - start) * 1000) / CLOCKS_PER_SEC;

	rewind(fin);

	start = clock();
	func3(fin, fout);
	end = clock();
	s3 = ((double)(end - start) * 1000) / CLOCKS_PER_SEC;

	fprintf(fout, "%lf\t%lf\t%lf", s1, s2, s3);  // 첫번째 함수, 두번째 함수, 세번째 함수의 실행 시간

	fclose(fin);
	fclose(fout);
}