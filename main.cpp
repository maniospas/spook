// g++ -o spook main.cpp -O3
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <cstring>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <memory>
#include <stdexcept>
#include <array>
#include <cmath>
#include <filesystem>

using namespace std;
typedef uint64_t u64; 
typedef uint32_t u32; 
typedef uint16_t u16; 
typedef double f64;
namespace syntax {
    const u64 eq = 0;
    const u64 lpar = 1;
    const u64 rpar = 2;
    const u64 lbra = 3;
    const u64 rbra = 4;
    const u64 comma = 5;
    const u64 dot = 6;
    const u64 lblk = 7;
    const u64 rblk = 8;
    const u64 cont = 9;
    const u64 let = 10;
    const u64 fn  = 11;
    const u64 arg = 12;
    const u64 del = 13;
    const u64 iff = 14;
    const u64 nadd = 15;
    const u64 nmul = 16;
    const u64 nsub = 17;
    const u64 ndiv = 18;
    const u64 fadd = 19;
    const u64 fmul = 20;
    const u64 fsub = 21;
    const u64 fdiv = 22;
    const u64 nnot = 23;
    const u64 nle  = 24;
    const u64 nge  = 25;
    const u64 nlt  = 26;
    const u64 ngt  = 27;
    const u64 neq  = 28;
    const u64 nneq = 29;
    const u64 fle  = 30;
    const u64 fge  = 31;
    const u64 flt  = 32;
    const u64 fgt  = 33;
    const u64 feq  = 34;
    const u64 fneq = 35;
    const u64 nprint = 36;
    const u64 fprint = 37;
    const u64 names = 38;
    const u64 ncast = 39;
    const u64 fcast = 40;
    const u64 sprint = 41;
    const u64 sadd = 42;
    const u64 then = 43;
    const u64 list = 44;
    const u64 lget = 45;
    const u64 ladd = 46;
    const u64 llen = 47;
    const u64 exec = 48;
    const u64 permit = 49;
    const u64 sin = 50;
    const u64 cos = 51;
    const u64 tan = 52;
    const u64 exp = 53;
    const u64 pow = 54;
    const u64 acos = 55;
    const u64 asin = 56;
    const u64 atan = 57;
    const u64 cosh = 58;
    const u64 sinh = 59;
    const u64 tanh = 60;
    const u64 open = 61;
    const u64 slen = 62;
    const u64 split = 63;
    const u64 sslice = 64;
    const u64 lslice = 65;
    const u64 sfind = 66;
    const u64 seq = 67;
    const u64 sneq = 68;
    const u64 write = 69;
    const u64 nconvert = 70;
    const u64 fconvert = 71;
    const u64 nstring = 72;
    const u64 fstring = 73;
    const u64 exists = 74;
}
namespace tags {
    const u16 isnat = 1;
    const u16 isfloat = 2;
    const u16 isstring = 3;
    const u16 isglobal = 4;
}

vector<vector<u64>> lists;
vector<vector<u64>> strings;
vector<FILE*> files;
unordered_map<string, u64> token2id;
vector<string> tokens;
const u64 STACK_SIZE = 1024*1024*4; // 4 MB
u64* stack = new u64[STACK_SIZE];
inline f64 toreal(u64 x) {f64 ret;std::memcpy(&ret,&x,sizeof(ret));return ret;}
inline u64 fromreal(f64 x) {u64 ret;std::memcpy(&ret,&x,sizeof(ret));return ret;}
inline u16 tou16(size_t x) { assert(x<65535); return static_cast<u16>(x); }
inline size_t id(const string& text) {
    auto found = token2id.find(text);
    if(found!=token2id.end()) return found->second;
    auto id = u64{tokens.size()};
    assert(id<65535);
    tokens.push_back(text);
    token2id[text] = id;
    return id;
}
void error(const string& description) {
    cout<<"\e[0;31mERROR\e[0m "<<description<<"\n";
    exit(1);
}
inline string totoken(u64 id) {
    if(id>=tokens.size()) error("unknown string");
    return tokens[id]; 
}

struct Token { u64 id; u16 tag; u16 row; u16 col; u16 fileid; };
void error(const string& description, const Token& tok) {
    cout<<"\e[0;31mERROR\e[0m "<<description<<"\n";
    auto endcol = tok.col;
    auto filename = totoken(tok.fileid);
    cout<<"   \e[0;33mat\e[0m "<<filename<<" line "<<tok.row<<"\n";
    // s
    exit(1);
}

#define ASSERT_SIZE(desired_size) if(arg_stack_end!=arg_stack_pos+desired_size)error("wrong number of input arguments to unpack in builtin function: "+totoken(func_name), program[error_token]);
#define ASSERT_MINSIZE(desired_size) if(arg_stack_end<arg_stack_pos+desired_size)error("not enough input arguments to unpack in builtin function: "+totoken(func_name), program[error_token]);
u64 call_function(const vector<Token>& program, const unordered_map<u64, u64>& func_positions, u64 name, u64 arg_stack_pos, u64 arg_stack_end, unordered_map<u64, u64>& globals, u64 error_tokens);
inline u64 process_expression(const vector<Token>& program, const unordered_map<u64, u64>& func_positions, u64& pos, unordered_map<u64, u64>& vars, unordered_map<u64, u64>& globals, u64 error_token, u64 arg_stack_pos) {
    if(pos>=program.size()) error("unexpected end of file", program[program.size()-1]);
    auto local_id = program[pos].id;
    if(program[pos].tag==tags::isglobal) {
        auto local = globals.find(local_id);
        if(local!=globals.end()) {++pos; return local->second;}
        error("failed to find namespace variable: "+totoken(local_id), program[pos]);
    }
    if(program[pos].tag) {++pos; return local_id;}
    auto local = vars.find(local_id);
    if(local!=vars.end()) {++pos; return local->second;}
    auto par_open = program[pos+1].id;
    if(pos+1<program.size() && (par_open==syntax::lpar || par_open==syntax::cont)) {
        auto func_name = local_id;
        auto func_pos = pos;
        auto arg_stack_end = arg_stack_pos;
        ++pos; 
        while(true) {
            ++pos;
            auto next_val = process_expression(program, func_positions, pos, vars, globals, error_token, arg_stack_end);
            stack[arg_stack_end] = next_val;
            arg_stack_end++;
            if(par_open==syntax::cont) break;
            if(pos>=program.size()) error("unexpected end of file", program[program.size()-1]);
            auto pos_id = program[pos].id;
            if(pos_id==syntax::rpar) {++pos;break;}
            if(pos_id!=syntax::comma) error("expecting comma or closing parenthesis by found: "+totoken(pos_id), program[pos]);
        }
        switch(func_name) {
            case syntax::nadd: {
                ASSERT_MINSIZE(2); 
                auto combined = stack[arg_stack_pos]+stack[arg_stack_pos+1];
                for(auto pos=arg_stack_pos+2;pos<arg_stack_end;++pos) combined += stack[pos];
                return combined;
            }
            case syntax::nmul: {
                ASSERT_MINSIZE(2); 
                auto combined = stack[arg_stack_pos]*stack[arg_stack_pos+1];
                for(auto pos=arg_stack_pos+2;pos<arg_stack_end;++pos) combined *= stack[pos];
                return combined;
            }
            case syntax::nsub: ASSERT_SIZE(2); if(stack[arg_stack_pos+1]>stack[arg_stack_pos]) error("negative natural substraction"); return stack[arg_stack_pos]-stack[arg_stack_pos+1];
            case syntax::ndiv: ASSERT_SIZE(2); if(stack[arg_stack_pos+1]==0) error("division by zero"); return stack[arg_stack_pos]/stack[arg_stack_pos+1];
            case syntax::nle: ASSERT_SIZE(2); return stack[arg_stack_pos]<=stack[arg_stack_pos+1];
            case syntax::nge: ASSERT_SIZE(2); return stack[arg_stack_pos]>=stack[arg_stack_pos+1];
            case syntax::nlt: ASSERT_SIZE(2); return stack[arg_stack_pos]<stack[arg_stack_pos+1];
            case syntax::ngt: ASSERT_SIZE(2); return stack[arg_stack_pos]>stack[arg_stack_pos+1];
            case syntax::neq: ASSERT_SIZE(2); return stack[arg_stack_pos]==stack[arg_stack_pos+1];
            case syntax::nneq: ASSERT_SIZE(2); return stack[arg_stack_pos]!=stack[arg_stack_pos+1];
            case syntax::nnot: ASSERT_SIZE(1); return stack[arg_stack_pos]==0;
            case syntax::nprint: ASSERT_SIZE(1); cout<<stack[arg_stack_pos]<<"\n"; return 0;
            case syntax::fadd: {
                ASSERT_MINSIZE(2); 
                auto combined = toreal(stack[arg_stack_pos])+toreal(stack[arg_stack_pos+1]);
                for(auto pos=arg_stack_pos+2;pos<arg_stack_end;++pos) combined += toreal(stack[pos]);
                return fromreal(combined);
            }
            case syntax::fmul: {
                ASSERT_MINSIZE(2); 
                auto combined = toreal(stack[arg_stack_pos])*toreal(stack[arg_stack_pos+1]);
                for(auto pos=arg_stack_pos+2;pos<arg_stack_end;++pos) combined *= toreal(stack[pos]);
                return fromreal(combined);
            }
            case syntax::fsub: ASSERT_SIZE(2); return fromreal(toreal(stack[arg_stack_pos])-toreal(stack[arg_stack_pos+1]));
            case syntax::fdiv: ASSERT_SIZE(2); return fromreal(toreal(stack[arg_stack_pos])/toreal(stack[arg_stack_pos+1]));
            case syntax::fle: ASSERT_SIZE(2); return toreal(stack[arg_stack_pos])<=toreal(stack[arg_stack_pos+1]);
            case syntax::fge: ASSERT_SIZE(2); return toreal(stack[arg_stack_pos])>=toreal(stack[arg_stack_pos+1]);
            case syntax::flt: ASSERT_SIZE(2); return toreal(stack[arg_stack_pos])<toreal(stack[arg_stack_pos+1]);
            case syntax::fgt: ASSERT_SIZE(2); return toreal(stack[arg_stack_pos])>toreal(stack[arg_stack_pos+1]);
            case syntax::feq: ASSERT_SIZE(2); return toreal(stack[arg_stack_pos])==toreal(stack[arg_stack_pos+1]);
            case syntax::fneq: ASSERT_SIZE(2); return toreal(stack[arg_stack_pos])!=toreal(stack[arg_stack_pos+1]);
            case syntax::fprint: ASSERT_SIZE(1); cout<<toreal(stack[arg_stack_pos])<<"\n"; return 0;
            case syntax::ncast: ASSERT_SIZE(1); return toreal(stack[arg_stack_pos]);
            case syntax::fcast: ASSERT_SIZE(1); return fromreal(stack[arg_stack_pos]);
            case syntax::sprint: ASSERT_SIZE(1); cout<<totoken(stack[arg_stack_pos])<<"\n"; return 0;
            case syntax::sadd: {
                ASSERT_MINSIZE(2); 
                auto combined = totoken(stack[arg_stack_pos])+totoken(stack[arg_stack_pos+1]);
                for(auto pos=arg_stack_pos+2;pos<arg_stack_end;++pos)
                    combined += totoken(stack[pos]);
                return id(combined);
            }
            case syntax::lget: {
                ASSERT_SIZE(2);
                auto list_index = stack[arg_stack_pos];
                if(list_index>=lists.size()) error("list #"+to_string(list_index)+" does not exist", program[func_pos]);
                auto& list = lists[list_index];
                auto get_index = stack[arg_stack_pos+1];
                if(list_index>=lists.size()) error("list index "+to_string(get_index)+" out of range", program[func_pos]);
                return list[get_index];
            }
            case syntax::list: {
                auto list_index = u64{lists.size()};
                lists.emplace_back();
                auto& list = lists[list_index];
                assert(arg_stack_pos<=arg_stack_end);
                list.reserve(arg_stack_end-arg_stack_pos);
                for(u64 pos=arg_stack_pos;pos<arg_stack_end;++pos)
                    list.push_back(stack[pos]);
                return list_index;
            }
            case syntax::ladd: {
                ASSERT_SIZE(2);
                auto list_index = u64{lists.size()};
                lists.emplace_back();
                auto& list = lists[list_index];
                for(u64 pos=arg_stack_pos;pos<arg_stack_end;++pos) {
                    auto list_index = stack[arg_stack_pos];
                    if(list_index>=lists.size()) error("list #"+to_string(list_index)+" does not exist", program[func_pos]);
                    auto& added_list = lists[list_index]; 
                    list.reserve(list.size()+distance(added_list.begin(),added_list.end()));
                    list.insert(list.end(),added_list.begin(),added_list.end());
                }
                return list_index;
            }
            case syntax::pow: ASSERT_SIZE(2); return fromreal(pow(toreal(stack[arg_stack_pos]), toreal(stack[arg_stack_pos+1])));
            case syntax::cos: ASSERT_SIZE(1); return fromreal(cos(toreal(stack[arg_stack_pos])));
            case syntax::sin: ASSERT_SIZE(1); return fromreal(sin(toreal(stack[arg_stack_pos])));
            case syntax::tan: ASSERT_SIZE(1); return fromreal(tan(toreal(stack[arg_stack_pos])));
            case syntax::cosh: ASSERT_SIZE(1); return fromreal(cosh(toreal(stack[arg_stack_pos])));
            case syntax::sinh: ASSERT_SIZE(1); return fromreal(sinh(toreal(stack[arg_stack_pos])));
            case syntax::tanh: ASSERT_SIZE(1); return fromreal(tanh(toreal(stack[arg_stack_pos])));
            case syntax::exp: ASSERT_SIZE(1); return fromreal(exp(toreal(stack[arg_stack_pos])));
            case syntax::atan: ASSERT_SIZE(1); return fromreal(atan(toreal(stack[arg_stack_pos])));
            case syntax::acos: ASSERT_SIZE(1); return fromreal(acos(toreal(stack[arg_stack_pos])));
            case syntax::asin: ASSERT_SIZE(1); return fromreal(asin(toreal(stack[arg_stack_pos])));
            
            case syntax::llen: {
                ASSERT_SIZE(1);
                auto list_index = stack[arg_stack_pos];
                if(list_index>=lists.size()) error("list #"+to_string(list_index)+" does not exist", program[func_pos]);
                return lists[list_index].size();
            }
            case syntax::slen: ASSERT_SIZE(1); return totoken(stack[arg_stack_pos]).size();
            case syntax::seq: ASSERT_SIZE(2); return totoken(stack[arg_stack_pos])==totoken(stack[arg_stack_pos]);
            case syntax::sneq: ASSERT_SIZE(2); return totoken(stack[arg_stack_pos])==totoken(stack[arg_stack_pos]);
            case syntax::nstring: ASSERT_SIZE(1); return id(to_string(stack[arg_stack_pos]));
            case syntax::fstring: ASSERT_SIZE(1); return id(to_string(toreal(stack[arg_stack_pos])));
            case syntax::exists: ASSERT_SIZE(1); return filesystem::exists(totoken(stack[arg_stack_pos]));
            case syntax::write: {
                ASSERT_SIZE(2); 
                auto path = totoken(stack[arg_stack_pos]);
                if(filesystem::exists(path)) error("file already exists: "+path, program[func_pos]);
                auto content = totoken(stack[arg_stack_pos+1]);
                auto out = ofstream{path};
                out << content;
                out.close();
                return 0;
            }
            case syntax::open: {
                auto ret = string{};
                if(arg_stack_end==arg_stack_pos) {
                    cin>>ret;
                    return id(ret);
                }
                ASSERT_SIZE(1); 
                auto path = totoken(stack[arg_stack_pos]);
                error_code ec;
                if(filesystem::is_directory(path, ec)) {
                    for(const auto & entry : filesystem::directory_iterator(path))
                        ret += entry.path().c_str()+string{"\n"};
                    return id(ret);
                }
                auto file = ifstream{path};
                if (!file.is_open()) error("file does not exist: "+path, program[func_pos]);
                auto line = string{};
                while (getline(file, line)) 
                    ret += line+"\n";
                file.close();
                return id(ret);
            }
            case syntax::nconvert: {
                ASSERT_SIZE(1); 
                u64 ret = 0;
                try{ret = stoull(totoken(stack[arg_stack_pos]));}catch(...){error("failed to convert to nat: "+totoken(stack[arg_stack_pos]), program[func_pos]);}
                return ret;
            }
            case syntax::fconvert: {
                ASSERT_SIZE(1); 
                u64 ret = 0;
                try{ret = stof(totoken(stack[arg_stack_pos]));}catch(...){error("failed to convert to float: "+totoken(stack[arg_stack_pos]), program[func_pos]);}
                return ret;
            }
            case syntax::exec: {
                ASSERT_SIZE(1);
                auto command = totoken(stack[arg_stack_pos]);
                array<char, 128> buffer;
                auto result = string{};
                unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
                if(!pipe) error("failed to open process", program[func_pos]);
                while(fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) 
                    result += buffer.data();
                return id(result);
            }
            default: break;
        }
        return call_function(program, func_positions, func_name, arg_stack_pos, arg_stack_end, globals, func_pos); 
    }
    error("failed to find local variable or function: "+totoken(local_id), program[pos<program.size()?pos:(program.size()-1)]);
    return 0;
}

u64 call_function(const vector<Token>& program, const unordered_map<u64, u64>& func_positions, u64 name, u64 arg_stack_pos, u64 arg_stack_end, unordered_map<u64, u64>& globals, u64 error_token) {
    auto vars = unordered_map<u64, u64>{};
    auto positer = func_positions.find(name);
    auto current_arg = arg_stack_pos;
    //auto vars = unordered_map<u64, u64>{};
    if(positer==func_positions.end()) error("cannot find function: "+totoken(name), program[error_token]);
    auto isnamespace = program[positer->second-1].id==syntax::names;
    for(auto pos = positer->second+1;pos<program.size();) {
        auto current_id = program[pos].id;
        if(current_id==syntax::fn || current_id==syntax::names) break;
        if(current_id==syntax::iff) {
            ++pos;
            auto result = process_expression(program, func_positions, pos, vars, globals, error_token, arg_stack_end);
            if(result!=0) {
                if(pos>=program.size()) error("unexpected end of file", program[program.size()-1]);
                if(program[pos].id!=syntax::then) error("expecting continuation symbol → but got: "+totoken(program[pos].id), program[pos]);
                pos++;
                return process_expression(program, func_positions, pos, vars, globals, error_token, arg_stack_end);
            }
            while(pos<program.size()) {
                current_id = program[pos].id;
                if(current_id==syntax::fn || current_id==syntax::names || current_id==syntax::iff || current_id==syntax::let || current_id==syntax::arg || current_id==syntax::del)
                    break;
                pos++;
            }
            continue;
        }
        if(current_id==syntax::let) {
            if(pos>=program.size()) return 0;
            ++pos;
            if(pos>=program.size()) error("unexpected end of file", program[program.size()-1]);
            auto var_name = program[pos].id;
            auto is_global = program[pos].tag==tags::isglobal;
            if(is_global) error("cannot externally set namespace variables", program[pos]);//globals[var_name] = result;
            ++pos;
            if(pos>=program.size()) error("unexpected end of file", program[program.size()-1]);
            if(program[pos].id!=syntax::eq) error("expecting assignment symbol = but got: "+totoken(program[pos].id), program[program.size()-1]);
            ++pos;
            auto result = process_expression(program, func_positions, pos, vars, globals, error_token, arg_stack_end);
            vars[var_name] = result;
            continue;
        }
        if(current_id==syntax::arg) {
            if(pos>=program.size()) return 0;//
            if(pos+1>=program.size()) error("unexpected end of file");
            if(isnamespace) error("namespaces cannot have arguments");
            ++pos;
            auto arg_name = program[pos].id;
            if(program[pos].tag==tags::isglobal) error("cannot set global variable as argument: "+totoken(arg_name), program[pos]);
            if(current_arg>=arg_stack_end) error("not enough input arguments to unpack: "+totoken(arg_name), program[error_token]);
            vars[arg_name] = stack[current_arg];
            ++current_arg;
            ++pos;
            continue;
        }
        error("unexpected syntax token: "+totoken(current_id));
    }
    if(isnamespace) for(const auto& [k,v]:vars) globals[id(tokens[name]+"::"+tokens[k])] = v;
    return 0;
}

u64 interpret(const vector<Token>& program) {
    // find function definitions and apply closure for variable names
    auto func_positions = unordered_map<u64, u64>{};
    auto globals = unordered_map<u64, u64>{};
    auto last_return = u64{0};
    for(size_t i=0;i<program.size()-1;++i) {
        auto current_pos = i;
        auto current_id = program[i].id;
        if(current_id!=syntax::fn && current_id!=syntax::names) continue;
        ++i;
        auto last_func_name = program[i].id;;
        if(func_positions.find(last_func_name)!=func_positions.end()) error("multiple definitions of: "+totoken(last_func_name), program[i]);
        func_positions[last_func_name] = i;
        if(current_id!=syntax::names) continue; // directly run namespace
        if(current_id==syntax::names) {
            last_return = call_function(program, func_positions, last_func_name, 0, 0, globals, current_pos);
            if(last_return!=0) return last_return;
        }
    }
    return 0;
}

int main(int argc, const char** argv) {
    auto data = vector<f64>{};
    if(argc<=1) error("missing source file");
    auto path = string{argv[1]};
    auto file = ifstream{"main.oo"};
    if(!file.is_open()) error("cannot open source: "+path);

    // TOKENIZE
    auto program = vector<Token>{};
    auto current_token = string{};
    string line;
    id("=");
    id("(");
    id(")");
    id("[");
    id("]");
    id(",");
    id(".");
    id("{");
    id("}");
    id("|");
    id("🎃");
    id("👻");
    id("🕸️");
    id("🧹");
    id("🍭");//if
    id("nadd");
    id("nmul");
    id("nsub");
    id("ndiv");
    id("fadd");
    id("fmul");
    id("fsub");
    id("fdiv");
    id("not");
    id("nle");
    id("nge");
    id("nlt");
    id("ngt");
    id("neq");
    id("nneq");
    id("fle");
    id("fge");
    id("flt");
    id("fgt");
    id("feq");
    id("fneq");
    id("nprint");
    id("fprint");
    id("🏚️"); // namespace
    id("ncast");
    id("fcast");
    id("sprint");
    id("sadd");
    id("→"); // then
    id("list");
    id("get");
    id("ladd");
    id("len");
    id("exec");
    id("🕯️"); // permit
    id("sin");
    id("cos");
    id("tan");
    id("exp");
    id("pow");
    id("acos");
    id("asin");
    id("atan");
    id("cosh");
    id("sinh");
    id("tanh");
    id("read");
    id("slen");
    id("split");
    id("sslice");
    id("lslice");
    id("sfind");
    id("seq");
    id("sneq");
    id("write");
    id("nconvert");
    id("fconvert");
    id("nstring");
    id("fstring");
    id("exists");

    auto fileid = tou16(id(path)); // file ids should be placed near the start 
    auto row = u16{0};
    while(getline(file, line)) {
        ++row;
        auto col = u16{0};
        auto in_string = false;
        for(auto c:line) {
            ++col;
            if(c=='\"') {
                if(!in_string) {
                    if(current_token.size()) {
                        u64 found_id;
                        u16 tag = 0;
                        if(current_token.find("::")!=string::npos) {found_id = id(current_token);tag=tags::isglobal;}
                        if(!tag && current_token.find(".")==string::npos) try{ found_id = u64{stoull(current_token.c_str())}; tag=tags::isnat; } catch(...) {}
                        if(!tag) try{ found_id = fromreal(stof(current_token.c_str())); tag=tags::isfloat; } catch(...) {}
                        if(!tag) found_id = id(current_token);
                        program.push_back(Token{.id=found_id, .tag=tag, .row=row, .col=col, .fileid=fileid});
                        current_token = "";
                    }
                    in_string = true;
                    continue;
                }
                // if in string
                in_string = false;
                auto found_id = id(current_token);
                auto tag=tags::isstring;
                program.push_back(Token{.id=found_id, .tag=tag, .row=row, .col=col, .fileid=fileid});
                current_token = "";
                continue;
            }
            if(in_string) {
                current_token += c;
                continue;
            }
            if(c=='=' || c=='|' || c=='(' || c==')' || c=='[' || c==']' || c==',' || c=='{' || c=='}' || c==' ' || c=='\t' || c=='#' || col>line.size()) {
                if(c=='.') {
                    bool valid_number_so_far = false;
                    try{ stoull(current_token.c_str()); valid_number_so_far=true; } catch(...) {}
                    if(valid_number_so_far) {
                        current_token += c;
                        continue;
                    }
                }
                if(current_token.size()) {
                    u64 found_id;
                    u16 tag = 0;
                    if(current_token.find("::")!=string::npos) {found_id = id(current_token);tag=tags::isglobal;}
                    if(!tag && current_token.find(".")==string::npos) try{ found_id = u64{stoull(current_token.c_str())}; tag=tags::isnat; } catch(...) {}
                    if(!tag) try{ found_id = fromreal(stof(current_token.c_str())); tag=tags::isfloat; } catch(...) {}
                    if(!tag) found_id = id(current_token);
                    program.push_back(Token{.id=found_id, .tag=tag, .row=row, .col=col, .fileid=fileid});
                    current_token = "";
                }
                if(c=='#') break; // line comments
                switch(c) {
                    case '=': program.push_back(Token{.id=0, .row=row, .col=col, .fileid=fileid});break;
                    case '(': program.push_back(Token{.id=1, .row=row, .col=col, .fileid=fileid});break;
                    case ')': program.push_back(Token{.id=2, .row=row, .col=col, .fileid=fileid});break;
                    case '[': program.push_back(Token{.id=3, .row=row, .col=col, .fileid=fileid});break;
                    case ']': program.push_back(Token{.id=4, .row=row, .col=col, .fileid=fileid});break;
                    case ',': program.push_back(Token{.id=5, .row=row, .col=col, .fileid=fileid});break;
                    case '.': program.push_back(Token{.id=6, .row=row, .col=col, .fileid=fileid});break;
                    case '{': program.push_back(Token{.id=7, .row=row, .col=col, .fileid=fileid});break;
                    case '}': program.push_back(Token{.id=8, .row=row, .col=col, .fileid=fileid});break;
                    case '|': program.push_back(Token{.id=9, .row=row, .col=col, .fileid=fileid});break;
                    default: break;
                }
                continue;
            }
            current_token += c;
        }
        if(current_token.size()) {
            u64 found_id;
            u16 tag = 0;
            
            if(current_token.find("::")!=string::npos) {found_id = id(current_token);tag=tags::isglobal;}
            if(!tag && current_token.find(".")==string::npos) try{ found_id = u64{stoull(current_token.c_str())}; tag=tags::isnat; } catch(...) {}
            if(!tag) try{ found_id = fromreal(stof(current_token.c_str())); tag=tags::isfloat;} catch(...) {}
            if(!tag) found_id = id(current_token);
            program.push_back(Token{.id=found_id, .tag=tag, .row=row, .col=col, .fileid=fileid});
            current_token = "";
        }
        if(in_string) error("strings cannot span multiple lines", program[program.size()-1]);
    }
    file.close();
    assert(current_token.size()==0);

    // PROCESS TOKENS
    return interpret(program);
}