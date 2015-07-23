#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <stack>
#include <cstring>

//文字列の複製
void strcpy(char*a,char*b){
	while(*a++=*b++);
}

//文字列のスペースをすべて取り除く
void trim(char*str){
	if(' '==*str) strcpy(str,str+1);
	if(*str++) trim(str);
}

//文字列をトークンごとに切り出し
void token(char*str,std::vector<std::string>*data){
	char pos;
	std::string strpos="";
	while(pos=*str++){
		switch (pos){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			strpos=strpos+pos;
			break;
		case '+':
			if(strpos!="") data->push_back(strpos);
			strpos="";
			data->push_back("+");
			break;
		case '-':
			if(strpos!="") data->push_back(strpos);
			strpos="";
			data->push_back("-");
			break;
		case '*':
			if(strpos!="") data->push_back(strpos);
			strpos="";
			data->push_back("*");
			break;
		case '/':
			if(strpos!="") data->push_back(strpos);
			strpos="";
			data->push_back("/");
			break;
		case '(':
			if(strpos!="") data->push_back(strpos);
			strpos="";
			data->push_back("(");
			break;
		case ')':
			if(strpos!="") data->push_back(strpos);
			strpos="";
			data->push_back(")");
			break;
		default:
			break;
		}
	}
	if(strpos!="") data->push_back(strpos);
}

//トークンであるか確認
bool isToken(std::string str){
	if(str=="+" || str=="-" || str=="*" || str=="/") return true;
	return false;
}

//トークンの整形する
void tokenAna(std::vector<std::string>*in,std::vector<std::string>*out){
	if(isToken((*in)[0])){
		(*in)[1]=(*in)[0]+(*in)[1];
	}else{
		out->push_back((*in)[0]);
	}

	for(int i=1;i<in->size();i++) {
		if(isToken((*in)[i])){
			if(isToken((*in)[i-1])){
				(*in)[i+1]=(*in)[i]+(*in)[i+1];
			}else{
				if((*in)[i-1]=="("){
					(*in)[i+1]=(*in)[i]+(*in)[i+1];
				}else{
					out->push_back((*in)[i]);
				}
			}
		}else{
			out->push_back((*in)[i]);
		}
	}
}



//逆ポーランド法に変換するための演算子の優先順位を決定
int Priority(const char*str){
	if(0==strcmp(str,"+")) return 10;
	if(0==strcmp(str,"-")) return 10;
	if(0==strcmp(str,"*")) return 20;
	if(0==strcmp(str,"/")) return 20;
	return 0;
}

//数式を逆ポーランド法に変換
void Analyzer(std::vector<std::string>*data,std::vector<std::string>*out,int*i){
	std::stack<const char*> stack;

	for(;*i<data->size();(*i)++){
		if((*data)[*i]=="+" || (*data)[*i]=="-" || (*data)[*i]=="*" || (*data)[*i]=="/"){
			while(stack.empty()==false && //false : スタックに値がある
				!(Priority((*data)[*i].c_str())>Priority(stack.top()))){
					const char*str=stack.top();//最後尾の値の取得
					stack.pop();//最後尾の値の削除
					out->push_back(str);
			}
			stack.push((*data)[*i].c_str());//値の追加
		}else{
			if((*data)[*i]=="("){
				*i=*i+1;
				Analyzer(data,out,i);
			}else{
				if((*data)[*i]==")"){
					goto end;
				}else{
					out->push_back((*data)[*i]);
				}
			}
		}
	}
end:
	while(stack.empty()==false){//false : スタックに値がある
		const char*str=stack.top();//最後尾の値の取得
		stack.pop();//最後尾の値の削除
		out->push_back(str);
	}
}
#include <time.h>
#include <iostream>
//逆ポーランド法表記の計算式を計算
double calc(std::vector<std::string>*out){
	clock_t bgn = clock();
	int c=out->size();
	std::stack<double> stack;
	double j,k;
	for(int i=0;i<c;i++){
			if((*out)[i]== "+"){
				j=stack.top();
				stack.pop();
				k=stack.top();
				stack.pop();
				stack.push(k+j);
			}else if((*out)[i]=="-"){
				j=stack.top();
				stack.pop();
				k=stack.top();
				stack.pop();
				stack.push(k-j);
			}else if((*out)[i]=="*"){
				j=stack.top();
				stack.pop();
				k=stack.top();
				stack.pop();
				stack.push(k*j);
			}else if((*out)[i]=="/"){
				j=stack.top();
				stack.pop();
				k=stack.top();
				stack.pop();
				stack.push(k/j);
			}else{
				stack.push(atof((*out)[i].c_str()));
			}
		}
	printf("%lf\n", ((double)(clock() - bgn) / CLOCKS_PER_SEC));
	return stack.top();
}

int main(){
	printf("%s\n","((-10+-20)*((30+40))+1)*2 [Enter] の様に入力してください");
	printf("--------------------------------------\n");
	while(1){
		std::vector<std::string> data;
		std::vector<std::string> out;

		//文字列として入力
		char str[1024];
		scanf("%s",str);
		trim(str);
		token(str,&data);

		std::vector<std::string> data2;
		tokenAna(&data,&data2);

		{printf("トークン切り出し結果 : ");for(int a=0;a<data2.size();a++) printf("%s ",data2[a].c_str());}

		int c=0;
		Analyzer(&data2,&out,&c);

		{printf("\n逆ポーランド法に変換 : ");for(int a=0;a<out.size();a++) printf("%s ",out[a].c_str());}
		printf("\n計算結果 : ");printf("%g\n",calc(&out));
		printf("--------------------------------------\n");
	}
	getchar();
	return 0;
}
