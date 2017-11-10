#include "ImportData.hpp"

ImportData::ImportData(string nome)
{
    string input;
    int cityId{0};
    cout << "Importando Arquivo \n";
    myfile.open(nome);
    if(!myfile.is_open()){
        cout <<"Falha na leitura do arquivo" <<endl;
        exit(EXIT_FAILURE);
    }
    myfile >> input;
    regexManager(input);
    while (myfile >> input)
    {
        regexManager(input);
        if (findIgnoredWords(input))
        {
            reader(flag, input);
        }
    }
}

void ImportData::regexManager(string input)
{
    //|[[:digit:]]+?=[\\.]?=[[:digit:]]+ 
    regex number("[[:digit:]]+");
    regex name("NAME(.*)");
    regex type("TYPE(.*)");
    regex comment("COMMENT(.*)");
    regex dimension("DIMENSION(.*)");
    regex edge_type("EDGE(.*)");
    regex node_comment("NODE(.*)");
    if (regex_match(input, name))
    {
        flag = "name";
        //cout << "Nome\n";
    }
    else if (regex_match(input, type))
    {
        flag = "type";
        //cout << "Tipo\n";
    }
    else if (regex_match(input, comment))
    {
        flag = "comment";
    }
    else if (regex_match(input, dimension))
    {
        flag = "dimension";
    }
    else if (regex_match(input, edge_type))
    {
        flag = "edge_type";
    }
    else if (regex_match(input, node_comment))
    {
        flag = "node_comment";
    }
    else if (regex_match(input, number) && flagaux == "wait for number")
    {
        flag = "number";
    }
}

bool ImportData::findIgnoredWords(string input)
{
    regex ignore(":|NAME(.*)|TYPE(.*)|COMMENT(.*)|DIMENSION(.*)|EDGE(.*)|EOF");
    if (regex_match(input, ignore))
    {
        return false;
    }
    //  cout << input;
    return true;
}

void ImportData::reader(string flag, string input)
{
    if (flag == "name")
    {
        tspName=input;
        //cout << input << " is a name! \n";
    }
    else if (flag == "type")
    {
        type=input;
        //cout << input << " is a type! \n";
    }
    else if (flag == "comment")
    {
        comment+=(input +" ");
        //cout << input << " is a comment! \n";
    }
    else if (flag == "dimension")
    {
        //cout << input << " is a dimension! \n";
        citiescoord.reserve(std::stoi(input));
        flagaux = "wait for number";
    }
    else if (flag == "edge_type")
    {
        edge_type=input;
        //cout << input << " is an edge_type! \n";
    }
    else if (flag == "node_comment")
    {
        node_comment=input;
        //cout << input << " is an node_comment! \n";
    }
    else if (flag == "number" && flagaux == "wait for number")
    {
        int id = std::stod(input);
            //cout << "I: " << input;
        myfile >> input;
        double x = std::stod(input);
            //cout << "\t X: " << input;
        myfile >> input;
        double y = std::stod(input);
            //cout << "\t Y: " << input << "\n";
        citiescoord.push_back(City(id,x,y));
    }
}
void ImportData::printInfos(){
    cout <<"Nome: " << gettspName() <<endl;
    cout<< "Tipo: " <<getType()<<endl;
    cout<< "Comentário: "<<getcomment()<<endl;
    cout<< "Tipo do Vértice: "<< getedge_type()<<endl;
    cout<< "Comentário do tipo de node: "<<getnode_comment()<<endl;
}
string ImportData::getInfos(){
    string input;
    cout << "BATATA";
    input ="Nome: " + gettspName()+"\n";
    input=input+"Tipo: " +getType()+"\n";
    input=input+"Comentário: "+getcomment()+"\n";
    input=input+"Tipo do Vértice: "+getedge_type()+"\n";
    input=input+"Comentário do tipo de node: "+getnode_comment()+"\n";
    cout<< input;
    return input;
}

vector<City> ImportData::getCitiesCoord(){
    return citiescoord;
}
string ImportData::gettspName(){
    return tspName;
}
string ImportData::getType(){
    return type;
}
string ImportData::getedge_type(){
    return edge_type;
}
string ImportData::getnode_comment(){
    return node_comment;
}
string ImportData:: getcomment(){
    if(comment==""){
        return "N/A";
    }else{
        return comment;
    }
}