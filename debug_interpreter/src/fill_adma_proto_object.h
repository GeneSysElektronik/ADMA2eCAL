#include <vector>
#include <ADMA.pb.h>

void fill_adma_proto_object(const std::vector<char>& input, pb::ADMA::ADMA_net& adma);
void fill_adma_proto_object(const std::vector<char>& input, pb::ADMA::ADMA_delta& adma);