#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// **ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ 1번 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ**

typedef struct {
	int degree;      // 최고 차수
	int capacity;    // coef 배열의 크기
	float* coef;     // 동적 할당된 계수 배열
} polynomial;


polynomial Zero(int capacity) {
	polynomial p;
	p.degree = 0;
	p.capacity = capacity;
	p.coef = (float*)calloc(capacity, sizeof(float)); // 0으로 초기화
	return p;
}

int IsZero(polynomial p) {
	for (int i = p.capacity - 1; i >= 0; i--) {
		if (p.coef[i] != 0) return 0;
	}
	return 1;
}

int COMPARE(int a, int b) {  // 비교하는 함수(초기버전의 차수 비교, 개선된 버전의 인덱스 비교에 사용)
	if (a > b) return 1;
	else if (a < b) return -1;
	else return 0;
}

int Lead_Exp(polynomial p) {
	for (int i = p.capacity - 1; i >= 0; i--) {
		if (p.coef[i] != 0) return i;
	}
	return 0;
}

float Coef(polynomial p, int a) {
	if (a >= p.capacity) return 0;
	return p.coef[a];
}

polynomial Remove(polynomial p, int a) {
	if (a >= p.capacity) return p;

	p.coef[a] = 0;
	if (a == p.degree) {
		for (int i = a - 1; i >= 0; i--) {
			if (p.coef[i] != 0) {
				p.degree = i;
				return p;
			}
		}
		p.degree = 0;
	}
	return p;
}

polynomial attach1(polynomial p, float a, int b) {
	if (b >= p.capacity) {
		// 배열 크기 확장
		int new_capacity = (b + 1) * 2;
		float* new_coef = (float*)calloc(new_capacity, sizeof(float));
		for (int i = 0; i < p.capacity; i++) {
			new_coef[i] = p.coef[i];
		}
		free(p.coef);
		p.coef = new_coef;
		p.capacity = new_capacity;
	}

	p.coef[b] += a;
	if (b > p.degree) p.degree = b;
	return p;
}

polynomial Padd(polynomial a, polynomial b) {
	// 초기 용량은 두 다항식 중 더 큰 용량으로 설정
	int init_capacity = (a.capacity > b.capacity ? a.capacity : b.capacity);
	polynomial d = Zero(init_capacity);

	float sum = 0.0f;

	while (IsZero(a) != 1 && !IsZero(b) != 1) {
		int exp_a = Lead_Exp(a);
		int exp_b = Lead_Exp(b);

		switch (COMPARE(exp_a, exp_b)) {
		case -1:
			d = attach1(d, Coef(b, exp_b), exp_b);
			b = Remove(b, exp_b);
			break;
		case 0:
			sum = Coef(a, exp_a) + Coef(b, exp_b);
			if (sum != 0.0f) {
				d = attach1(d, sum, exp_a);
			}
			a = Remove(a, exp_a);
			b = Remove(b, exp_b);
			break;
		case 1:
			d = attach1(d, Coef(a, exp_a), exp_a);
			a = Remove(a, exp_a);
			break;
		}
	}

	// 남은 항 더하기
	while (IsZero(a) != 1) {
		int exp = Lead_Exp(a);
		d = attach1(d, Coef(a, exp), exp);
		a = Remove(a, exp);
	}

	while (IsZero(b) != 1) {
		int exp = Lead_Exp(b);
		d = attach1(d, Coef(b, exp), exp);
		b = Remove(b, exp);
	}

	return d;
}

void FreePolynomial(polynomial* p) {
	if (p->coef != NULL) {
		free(p->coef);
		p->coef = NULL;
	}
	p->capacity = 0;
	p->degree = 0;
}

polynomial CopyPolynomial(polynomial p) {
	polynomial copy = Zero(p.capacity);
	copy.degree = p.degree;
	for (int i = 0; i < p.capacity; i++) {
		copy.coef[i] = p.coef[i];
	}
	return copy;
}


void PrintPolynomial(FILE* f, polynomial p) {
	int first = 0;
	for (int i = p.capacity - 1; i >= 0; i--) {
		if (p.coef[i] != 0) {
			if (first != 0) {
				fprintf(f, " + ");
			}
			fprintf(f, "%.0fx^%d", p.coef[i], i);
			first = 1;
		}
	}
	if (first == 0) fprintf(f, "0");
	fprintf(f, "\n");
}
// **ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ 2번 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ**

typedef struct {
	float coef; // 계수
	int exp; // 지수
} polynomial2;

polynomial2* terms; // 전역 변수 배열 
int avail = 0;  // 다음 항이 들어갈 인덱스
int capacity = 1;

void newTerms(int new_cap) {
	terms = (polynomial2*)calloc(new_cap, sizeof(polynomial2));
	if (!terms) {
		fprintf(stderr, "메모리 할당 실패\n");
		exit(1);
	}
	capacity = new_cap;
}

void bigger() {
	int new_capacity = capacity * 2;
	polynomial2* temp = (polynomial2*)realloc(terms, sizeof(polynomial2) * new_capacity);

	terms = temp;
	capacity = new_capacity;
}

void attach2(float coefficient, int exponent)
{
	if (avail >= capacity) {
		bigger();
	}
	terms[avail].coef = coefficient;
	terms[avail].exp = exponent;
	avail++;
}

void padd2(int starta, int finisha, int startb, int finishb, int* startd, int* finishd)
{
	float sum;
	*startd = avail;  // 결과 다항식 시작 인덱스 설정

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

	*finishd = avail - 1;  // 결과 다항식 끝 인덱스 설정
}



void PrintPolynomial2(FILE* f, int start, int finish) {
	for (int i = start; i <= finish - 1; i++) {  // 내림차순으로 정렬
		for (int j = i + 1; j <= finish; j++) {
			if (terms[i].exp < terms[j].exp) {
				polynomial2 temp = terms[i];
				terms[i] = terms[j];
				terms[j] = temp;
			}
		}
	}

	int first = 0;
	for (int i = start; i <= finish;) {
		float coef_sum = terms[i].coef;
		int exp = terms[i].exp;
		int j = i + 1;

		// 혹시라도 합이 안될 수도 있으니 한 번더 체크한다 

		while (j <= finish && terms[j].exp == exp) {
			coef_sum += terms[j].coef;
			j++;
		}

		if (coef_sum != 0) {
			if (first) fprintf(f, " + ");
			fprintf(f, "%.0fx^%d", coef_sum, exp);
			first = 1;
		}

		i = j;
	}

	if (first == 0) fprintf(f, "0");
	fprintf(f, "\n");
}
// **ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ 3번 ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ**

typedef struct polyNode* polyPointer;
typedef struct polyNode {
	float coef;
	int exp;
	polyPointer link;
} polyNode;

void attach3(float coefficient, int exponent, polyPointer* ptr) {  // 3번째 다항식 더하기
	polyPointer temp;
	temp = (polyPointer)malloc(sizeof(struct polyNode));  // 동적 배열
	temp->coef = coefficient;
	temp->exp = exponent;
	temp->link = NULL;
	(*ptr)->link = temp;  // 현재 노드 뒤에 연결
	*ptr = temp;  // 현재 포인터를 새 노드로 바꾼다
}

polyPointer padd(polyPointer a, polyPointer b) {
	polyPointer s, rear, temp;
	int sum;

	rear = (polyPointer)malloc(sizeof(struct polyNode));
	rear->link = NULL;
	s = rear;

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

	polyPointer result = s->link;
	free(s);  // dummy node만 제거
	return result;

}

void freeList(polyPointer head) { // 동적할당한거 없애주기 free()
	polyPointer temp;
	while (head != NULL) {
		temp = head;
		head = head->link;
		free(temp);
	}
}

void SortPolynomial(polyPointer head) {  // exp 기준 내림차순으로 정렬
	if (head == NULL) return;
	int count = 0;  // 무한 루프 방지용

	for (polyPointer i = head; i->link != NULL; i = i->link) {
		for (polyPointer j = i->link; j != NULL; j = j->link) {
			if (i->exp == j->exp) {
				i->coef += j->coef;
				j->coef = 0;
			}
			if (i->exp < j->exp) {   // 정렬
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

	// 적절한 초기 용량 설정 (최소 1 이상)
	int cap1 = x1 > 0 ? x1 * 2 : 4;
	int cap2 = x2 > 0 ? x2 * 2 : 4;

	polynomial a = Zero(cap1);
	polynomial b = Zero(cap2);

	for (int i = 0; i < x1; i++) {
		float coef;
		int exp;
		fscanf(fin, "%f%d", &coef, &exp);
		a = attach1(a, coef, exp);
	}

	for (int i = 0; i < x2; i++) {
		float coef;
		int exp;
		fscanf(fin, "%f%d", &coef, &exp);
		b = attach1(b, coef, exp);
	}

	polynomial a_copy = CopyPolynomial(a);
	polynomial b_copy = CopyPolynomial(b);
	polynomial result1 = Padd(CopyPolynomial(a), CopyPolynomial(b));

	// 출력
	PrintPolynomial(fout, a);
	PrintPolynomial(fout, b);
	PrintPolynomial(fout, result1);

	// 메모리 해제
	FreePolynomial(&a);
	FreePolynomial(&b);
	FreePolynomial(&a_copy);
	FreePolynomial(&b_copy);
	FreePolynomial(&result1);
}

void func2(FILE* fin, FILE* fout) {

	int x1, x2;
	fscanf(fin, "%d%d", &x1, &x2);

	newTerms(capacity);
	avail = 0; // 전역 배열 초기화

	int starta = avail;

	for (int i = 0; i < x1; i++) {   // 다항식 a 입력
		float coef;
		int exp;
		fscanf(fin, "%f%d", &coef, &exp);
		attach2(coef, exp);
	}
	int finisha = avail - 1;

	int startb = avail;
	for (int i = 0; i < x2; i++) {    // 다항식 b 입력
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

	free(terms);

	return;
}

void func3(FILE* fin, FILE* fout) {
	int x1, x2;
	fscanf(fin, "%d%d", &x1, &x2); // 각 다항식의 항 개수

	polyPointer a = NULL, a_tail = NULL;  // 다항식 a 입력
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

	polyPointer b = NULL, b_tail = NULL;  // 다항식 b 입력 
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

	freeList(a);
	freeList(b);
	freeList(c);

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
	s1 = ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;

	rewind(fin);  // 읽는 포인터 위치 초기화

	start = clock();
	func2(fin, fout);
	end = clock();
	s2 = ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;

	rewind(fin);

	start = clock();
	func3(fin, fout);
	end = clock();
	s3 = ((double)(end - start) * 1000.0) / CLOCKS_PER_SEC;

	fprintf(fout, "%.4lf\t%.4lf\t%.4lf\n", s1, s2, s3);

	fclose(fin);
	fclose(fout);
}