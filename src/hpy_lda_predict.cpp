#include <string>
#include "cmdline.h"
#include "random_util.hpp"
#include "hpy_lda_model.hpp"
#include "log.hpp"
#include "particle_filter_document_manager.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  cmdline::parser p;
  p.add<string>("file", 'f', "test file", true);
  p.add<int>("particles", 'p', "particle num", false, 1);
  p.add<int>("step", 's', "reestimate step size", false, 1);
  p.add<string>("model", 'm', "model file name (not directory)", true);
  p.parse_check(argc, argv);

  try {
    hpy_lda::init_rnd();
    
    auto model = hpy_lda::LoadModel<hpy_lda::HpyLdaSampler>(p.get<string>("model"));
    cerr << "model load done!" << endl;

    auto& sampler = model.sampler();
    auto ct_analyzer = sampler.GetCTAnalyzer();
    
    cerr << "tree node size: " << ct_analyzer.CountNodes() << endl;

    auto pf_dmanager = model.GetPFDocumentManager(p.get<int>("particles"));
    pf_dmanager.Read(p.get<string>("file"));
    auto pf_sampler = sampler.GetParticleFilterSampler(pf_dmanager);

    double ppl = pf_sampler.Run(p.get<int>("step"), cout);
    cerr << "PPL: " << ppl << endl;
    cout << "PPL: " << ppl << endl;
  } catch (string& what) {
    cerr << what << endl;
    return 1;
  }
  return 0;
}
