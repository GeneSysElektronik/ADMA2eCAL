#include <vector>
#include <Adma.pb.h>

void fill_adma_proto_object(const std::vector<char>& input, pb::ADMA::ADMA_net& adma);
void fill_adma_proto_object(const std::vector<char>& input, pb::ADMA::ADMA_delta& adma);
