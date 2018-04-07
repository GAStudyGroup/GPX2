#include "GPX2Fusion.hpp"

void GPX2Fusion::checkUnfeasiblePartitions(GPX2Structs::PartitionMap &fpartition, GPX2Structs::PartitionMap &ufpartition, GPX2Structs::NodeMap &red, GPX2Structs::NodeMap &blue)
{ // Função que irá verificar se foi gerada alguma partição feasible durante a fusão
    for (auto it = ufpartition.begin(); it != ufpartition.end();) {
        if (GPX2Support::checkPartition((*it).second,red,blue)) { // Checar se é feasible
            // se for irá ser retirada das partições unfeasible e colocada nas partições feasible
            fpartition.insert(make_pair((*it).first, (*it).second));
            it = ufpartition.erase(it);
        } else {
            it++;
        }
    }
}

bool GPX2Fusion::comparePairInt(const GPX2Structs::UnfeasibleConnection& p1, const GPX2Structs::UnfeasibleConnection& p2)
{
    bool first = p1.partitionId1 == p2.partitionId1 || p1.partitionId1 == p2.partitionId2;
    bool second = p1.partitionId2 == p2.partitionId1 || p1.partitionId2 == p2.partitionId2;

    return ((first && second));
}

bool GPX2Fusion::compareUnfeasibleConnections(const GPX2Structs::UnfeasibleConnection& uf1, const GPX2Structs::UnfeasibleConnection& uf2)
{
    bool first = ((uf1.partitionId1 == uf2.partitionId1) || (uf1.partitionId1 == uf2.partitionId2));
    bool second = ((uf1.partitionId2 == uf2.partitionId1) || (uf1.partitionId2 == uf2.partitionId2));

    return (first && second);
}

void GPX2Fusion::countConnectedPartitions(GPX2Structs::PartitionMap &partitions)
{ // Função que irá realizar a contagem das ligações entre as partições
    for (auto& p : partitions) {
        map<int, int> countConnected;
        for (auto node : p.second->getConnectedTo()) {
            // Tenta inserir uma nova partição que está conectada
            auto result = countConnected.insert(make_pair(node.connectedPartition, 1));
            // Caso a inserção falhar, a partição já está na lista de conectadas, sendo assim o total de ligações será aumentado
            if (!result.second) {
                countConnected[node.connectedPartition]++;
            }
        }
        // Insere os resultados da contagem
        for (auto node : countConnected) {
            p.second->getConnections().push_back(make_pair(node.first, node.second));
        }
    }
}

void GPX2Fusion::fusePartitions(vector<GPX2Structs::UnfeasibleConnection> &fuseWith,GPX2Structs::PartitionMap &partitions, GPX2Structs::NodeMap &unitedGraph, GPX2Structs::NodeMap& red, GPX2Structs::NodeMap& blue)
{ // Realiza a fusão entre as partições, de acordo com as validações já realizadas
    // Início da execução da fusão em si
    for (auto it = fuseWith.begin(); it != fuseWith.end();) {
        // Não precisa mudar o custo pois a checkPartition não verifica isso
        Partition* p1Ptr = partitions[(*it).partitionId1];
        Partition* p2Ptr = partitions[(*it).partitionId2];
        vector<string> intermediariesNodes, accessNodesToRemove;

        // Deletar as entradas de conexões (connectTo) diferentes da partição que será fundida
        for (auto itIn = p1Ptr->getConnectedTo().begin(); itIn != p1Ptr->getConnectedTo().end();) {
            if ((*itIn).connectedPartition != (*it).partitionId2) {
                p1Ptr->getConnectedTo().erase(itIn);
            } else {
                itIn++;
            }
        }
        for (auto itIn = p2Ptr->getConnectedTo().begin(); itIn != p2Ptr->getConnectedTo().end();) {
            if ((*itIn).connectedPartition != (*it).partitionId1) {
                p2Ptr->getConnectedTo().erase(itIn);
            } else {
                itIn++;
            }
        }

        for (string nodes : p2Ptr->getNodes()) {
            p1Ptr->getNodes().push_back(nodes);
        }
        for (string access : p2Ptr->getAccessNodes()) {
            p1Ptr->getAccessNodes().push_back(access);
        }

        // Percorrer os nós com ligação na partição que será feita a fusão
        for (auto connectedNode : p2Ptr->getConnectedTo()) {

            // Pegar os nós intermediários (o "caminho" da conexão)
            pair<GPX2Structs::SearchResult, vector<string>> tmp = GPX2Support::DFS_outside(connectedNode.node, partitions,unitedGraph);

            // Apagar os access nodes da lista da partição
            accessNodesToRemove.push_back(connectedNode.node);
            accessNodesToRemove.push_back(connectedNode.connectedNode);

            // Apagar o primeiro pois é o nó access que da origem a busca
            if (!tmp.second.empty()) {
                tmp.second.erase(tmp.second.begin());

                // Apagar o último nó pois é onde ele se conecta na partição que será unida
                if (!tmp.second.empty()) {
                    tmp.second.erase(tmp.second.end());
                }
            }

            // Carrega os nós intermediarios
            intermediariesNodes.insert(intermediariesNodes.end(), tmp.second.begin(), tmp.second.end());
        }

        /*  
            Inserir os nós intermediários na nova partição fundida
            Os nós são a ligação entre a P1 e P2, sendo a conexão de fusão das duas
        */
        p1Ptr->getNodes().insert(p1Ptr->getNodes().end(), intermediariesNodes.begin(), intermediariesNodes.end());

        // Os access nodes que ligavam as duas partições não serão mais access nodes, eles serão removidos
        GPX2Support::eraseSubVector(p1Ptr->getAccessNodes(), accessNodesToRemove);

        //atualizar a lista de entradas e saídas
        p1Ptr->setEntryAndExits(GPX2Support::getEntryAndExitList(p1Ptr,red,blue));

        // Apagar tudo
        delete p2Ptr;
        partitions.erase((*it).partitionId2);

        //apaga a entrada na fuseWith pois a fusão já foi realizada
        it = fuseWith.erase(it);
    }
}

vector<GPX2Structs::UnfeasibleConnection> GPX2Fusion::fusionsWithPartition(const int id, vector<GPX2Structs::UnfeasibleConnection>& connections)
{
    vector<GPX2Structs::UnfeasibleConnection> instances;
    for (const auto& con : connections) {
        if (con.partitionId1 == id || con.partitionId2 == id) {
            instances.push_back(con);
        }
    }
    return (instances);
}

void GPX2Fusion::generateFusionPairs(vector<GPX2Structs::UnfeasibleConnection> &fuseWith,GPX2Structs::PartitionMap &partition)
{ // Percorre o mapa com as partições unfeasible para gerar a lista com fusões a serem feitas

    for (auto uF : partition) {

        // Pega o retorno da função, IDs das partições e número de conexões entre elas, e salva na estrutura
        GPX2Structs::UnfeasibleConnection data = whichPartitionToFuseWith(partition[uF.first]);

        // Caso seja -1 ele está ligado a uma partição feasible, logo é ignorado
        if (data.partitionId1 != -1) {
            // Carrega o par de partições para fundir
            fuseWith.push_back(GPX2Structs::UnfeasibleConnection(uF.first, data.partitionId1, data.numberOfConnections));
        }
    }

    /* 
        Irá percorrer a lista fuseWith retirando as entradas iguais

        Ex: 1,2 e 2,1
            É uma situação de igualdade na qual é retirada, sobrando apenas uma
    */
    for (auto itOut = fuseWith.begin(); itOut != fuseWith.end(); itOut++) {
        for (auto itIn = fuseWith.begin(); itIn != fuseWith.end();) {
            if (itOut != itIn) {

                // Método para comparação, caso ocorra a igualdade então é retirado da lista
                if (comparePairInt(*itOut, *itIn)) {
                    itIn = fuseWith.erase(itIn);
                } else {
                    itIn++;
                }
            } else {
                itIn++;
            }
        }
    }

    for (const auto& p : partition) {
        // Pega o retorno do método, todas as possíveis fusões daquela partição
        vector<GPX2Structs::UnfeasibleConnection> tmp = fusionsWithPartition(p.first, fuseWith);

        // Se for menor que 2, logo não é possível que ela seja utilizada em mais de uma fusão.
        if (fuseWith.size() < 2) {
            break;
        }

        // Bloco para validação em caso de "melhor" fusão, compara entre as possíveis fusões e irá manter apenas a com maior número de conexões entre as partições.
        if (!tmp.empty()) {
            int maxCon{ -1 }, maxPos{ -1 };
            for (unsigned i = 0; i < tmp.size(); i++) {
                if (tmp[i].numberOfConnections > maxCon) {
                    maxCon = tmp[i].numberOfConnections;
                    maxPos = i;
                }
            }

            tmp.erase(tmp.begin() + maxPos);

            
            /*vector<UnfeasibleConnection> sameCon;
            int maxCon{ -1 }, maxPos{ -1 };
            for (unsigned i = 0; i < tmp.size(); i++) {
                if (tmp[i].numberOfConnections > maxCon) {
                    sameCon.clear();
                    maxCon = tmp[i].numberOfConnections;
                    maxPos = i;
                    sameCon.push_back(tmp[i]);
                }else if(tmp[i].numberOfConnections == maxCon){
                    sameCon.push_back(tmp[i]);
                }
            }

            if(sameCon.size()!=1){
                
                unsigned numberOfNodes{999999};
                unsigned numberOfNodes{0}; 
                UnfeasibleConnection best{0,0,0};
                for(UnfeasibleConnection uC : sameCon){
                    unsigned p1Nodes{unfeasiblePartitions[uC.partitionId1]->getNodes().size()},p2Nodes{unfeasiblePartitions[uC.partitionId2]->getNodes().size()};
                    unsigned sum{p1Nodes+p2Nodes};
                    //fusão que produzi o maior numero de nodes
                    if(sum>numberOfNodes){
                        numberOfNodes=sum;
                        best = uC;
                    } 

                    //fusão que produzi o maior numero de nodes
                    if(sum<numberOfNodes){
                        numberOfNodes=sum;
                        best = uC;
                    }
                }
                std::cout<<"escolheu a fusion com mais nodes "<<numberOfNodes<<endl; 
                std::cout<<"escolheu a fusion com menos nodes "<<numberOfNodes<<endl;
                tmp.erase(find_if(tmp.begin(),tmp.end(),[&best](UnfeasibleConnection const &uC){return((uC.partitionId1==best.partitionId1) && (uC.partitionId2==best.partitionId2));}));

            }else{
                tmp.erase(tmp.begin() + maxPos);
            } */

            for (auto& unfCon : tmp) {
                for (auto it = fuseWith.begin(); it != fuseWith.end();) {

                    if (compareUnfeasibleConnections(unfCon,(*it))) {
                        it = fuseWith.erase(it);
                    } else {
                        it++;
                    }
                }
            }
        }
    }
}

bool GPX2Fusion::unfeasiblePartitionsConnected(GPX2Structs::PartitionMap &partitions, GPX2Structs::NodeMap& unitedGraph)
{ // Verifica as conexões das partições unfeasible
  // And set the connectionNode in the partition
    vector<pair<string,int>> nodesToCheck;
    vector<string> nodesAlreadyChecked;
    bool atLeastOneConnected{ false };

    // Pega todos os nós que serão verificados
    for (auto& it : partitions) {
        for (string id : it.second->getAccessNodes()) {
            nodesToCheck.push_back(make_pair(id,it.first));
        }
    }

    // Percorre o vetor para verificar as conexões
    for (pair<string,int> node : nodesToCheck) {
        //não precisa verificar novamente se já foi encontrado aonde aquele nó chega
        if(find(nodesAlreadyChecked.begin(),nodesAlreadyChecked.end(),node.first)==nodesAlreadyChecked.end()){
            nodesAlreadyChecked.push_back(node.first);

            auto res = GPX2Support::DFS_outside(node.first, partitions, unitedGraph);
            
            int partitionConnected = GPX2Support::whichPartition(res.second.back(),partitions);
            
            partitions[node.second]->getConnectedTo().push_back(Partition::ConnectionNode(partitionConnected, node.first, res.second.back()));

            //utilizando o back podemos pegar a ultima conexão adicionada
            // Verifica qual partição está conectada
            int connectedPartition = partitions[node.second]->getConnectedTo().back().connectedPartition;

            // Verifica o ID do nó na qual ela está conectada
            string connectedId = partitions[node.second]->getConnectedTo().back().connectedNode;

            // Caso ele esteja conectado a outra partição unfeasible, caso não seja "-1"
            if (connectedPartition != -1) {
                atLeastOneConnected = true;

                // Insere no vetor de conexões
                partitions[connectedPartition]->getConnectedTo().push_back(Partition::ConnectionNode(node.second, connectedId, node.first));

                nodesAlreadyChecked.push_back(connectedId);
            } else {
                // Apaga por estar conectado com uma feasible
                partitions[node.second]->getConnectedTo().erase(partitions[node.second]->getConnectedTo().end());
            }
        }
    }
    return (atLeastOneConnected);
}

/* 
    - transformar vector<string> em vector<pair<string,pair>>, guardar o id da partição para evitar usar a whichPartition desnecessáriamente;
    - trocar o for com iterator por um for each e verificar apenas se já foi verificado para aquele nó;
 */

GPX2Structs::UnfeasibleConnection GPX2Fusion::whichPartitionToFuseWith(Partition* partition)
{ // Retorna um par com a partição que contém mais conexões e o total de conexões

    int partitionId{ -1 }, max{ -1 };
    for (auto p : partition->getConnections()) {
        if (p.second > max) {
            max = p.second;
            partitionId = p.first;
        }
    }
    return (GPX2Structs::UnfeasibleConnection(partitionId, max, max));
}

