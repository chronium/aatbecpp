//
// Created by chronium on 29.08.2022.
//

#include <jit.hpp>

using namespace llvm;
using namespace llvm::orc;

namespace aatbe::jit {

Expected<std::unique_ptr<AatbeJit>> AatbeJit::Create() {
  auto EPC = SelfExecutorProcessControl::Create();
  if (!EPC)
    return EPC.takeError();

  auto ES = std::make_unique<ExecutionSession>(std::move(*EPC));

  JITTargetMachineBuilder JTMB(
      ES->getExecutorProcessControl().getTargetTriple());

  auto DL = JTMB.getDefaultDataLayoutForTarget();
  if (!DL)
    return DL.takeError();

  return std::make_unique<AatbeJit>(std::move(ES), std::move(JTMB),
                                    std::move(*DL));
}

Error AatbeJit::addModule(llvm::orc::ThreadSafeModule TSM,
                          llvm::orc::ResourceTrackerSP RT) {
  if (!RT)
    RT = this->MainJD.getDefaultResourceTracker();
  return this->CompileLayer.add(RT, std::move(TSM));
}

Expected<JITEvaluatedSymbol> AatbeJit::lookup(StringRef Name) {
  return this->ES->lookup({&this->MainJD}, Name.str());
}

} // namespace aatbe::jit
