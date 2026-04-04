// -*- C++ -*-
//
// Package:    Analisis/MyAnalyzer
// Class:      MyAnalyzer
//
/**\class MyAnalyzer MyAnalyzer.cc Analisis/MyAnalyzer/plugins/MyAnalyzer.cc
 Description: Analyzer para extraer datos de FatJets y sus constituyentes (Point Cloud) y guardar CSV + ROOT
*/

// system include files
#include <memory>
#include <vector>
#include <string>
#include <cmath> // Necesario para std::abs
#include <fstream> // IMPORTANTE: Necesario para escribir el CSV
#include <iomanip> // --- CAMBIO: Para controlar la precision de los decimales

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TTree.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Common/interface/TriggerNames.h"

class MyAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit MyAnalyzer(const edm::ParameterSet&);
      ~MyAnalyzer() override;

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      edm::EDGetTokenT<std::vector<pat::Jet>> jetToken_;
      edm::EDGetTokenT<edm::TriggerResults> trigToken_;

      TTree *tree_;

      // --- VARIABLES GLOBALES DEL JET ---
      std::vector<float> j_pt, j_eta, j_phi, j_mass, j_SDmass, j_tau1, j_tau2;
      std::vector<float> j_tau21;

      // --- VARIABLES DE LA NUBE DE PARTICULAS (PF CANDIDATES) ---
      std::vector<std::vector<float>> p_pt, p_eta, p_phi, p_e;
      std::vector<std::vector<float>> p_dxy, p_dz;
      std::vector<std::vector<int>> p_pdgid;

      // --- VARIABLES PARA CSV ---
      std::string csvFileName_;
      std::ofstream csvFile_;
};

//
// constructors and destructor
//
MyAnalyzer::MyAnalyzer(const edm::ParameterSet& iConfig)
 : jetToken_(consumes<std::vector<pat::Jet>>(iConfig.getParameter<edm::InputTag>("jets"))),
   trigToken_(consumes<edm::TriggerResults>(edm::InputTag("TriggerResults", "", "HLT"))),
   csvFileName_(iConfig.getParameter<std::string>("outputCSV")) 
{
   usesResource("TFileService");
   edm::Service<TFileService> fs;
   tree_ = fs->make<TTree>("Events", "FatJet Point Cloud Data");

   tree_->Branch("FatJet_pt", &j_pt);
   tree_->Branch("FatJet_eta", &j_eta);
   tree_->Branch("FatJet_phi", &j_phi);
   tree_->Branch("FatJet_mass", &j_mass);
   tree_->Branch("FatJet_softdrop_mass", &j_SDmass);
   tree_->Branch("FatJet_tau1", &j_tau1);
   tree_->Branch("FatJet_tau2", &j_tau2);

   tree_->Branch("FatJet_tau21", &j_tau21);

   tree_->Branch("part_pt", &p_pt);
   tree_->Branch("part_eta", &p_eta);
   tree_->Branch("part_phi", &p_phi);
   tree_->Branch("part_e", &p_e);
   tree_->Branch("part_dxy", &p_dxy);
   tree_->Branch("part_dz", &p_dz);
   tree_->Branch("part_pdgid", &p_pdgid);
}

MyAnalyzer::~MyAnalyzer()
{
}

//
// member functions
//

void MyAnalyzer::beginJob()
{
    csvFile_.open(csvFileName_);
    // --- CAMBIO: Actualizamos la cabecera para incluir las arrays de constituyentes ---
    // Nota: Las columnas "PF_..." contendran strings con valores separados por ';'
    csvFile_ << "EventID,JetPt,JetEta,JetPhi,JetMass,SDMass,Tau1,Tau2,Tau21,"
             << "PF_Pt,PF_Eta,PF_Phi,PF_E,PF_PdgId,PF_Dxy,PF_Dz\n";
    
    // Configuramos precision para no perder datos en el CSV
    csvFile_ << std::fixed << std::setprecision(6);
}

void MyAnalyzer::endJob()
{
    if (csvFile_.is_open()) {
        csvFile_.close();
    }
}


void MyAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   edm::LogPrint("MyAnalyzer") << ">>> Procesando evento: " << iEvent.id().event();

   edm::Handle<edm::TriggerResults> h_trig;
   iEvent.getByToken(trigToken_, h_trig);
   if (!h_trig.isValid()) {
       edm::LogPrint("MyAnalyzer") << "ERROR: TriggerResults no valido!";
       return;
   }
   edm::LogPrint("MyAnalyzer") << "TriggerResults OK, size: " << h_trig->size();

   const edm::TriggerNames &names = iEvent.triggerNames(*h_trig);
   bool passHLT = false;
   for (unsigned int i = 0; i < h_trig->size(); ++i) {
       const std::string &trigName = names.triggerName(i);
       if (trigName.find("AK8") != std::string::npos) {
           edm::LogPrint("MyAnalyzer") << "  Trigger AK8: "
                                       << trigName
                                       << " | pass: " << h_trig->accept(i);
       }
       if (trigName.find("HLT_AK8PFJet450") != std::string::npos && h_trig->accept(i)) {
           passHLT = true;
           break;
       }
   }

   edm::LogPrint("MyAnalyzer") << "passHLT = " << passHLT;
   if (!passHLT) return;

   edm::Handle<std::vector<pat::Jet>> jets;
   iEvent.getByToken(jetToken_, jets);
   if (!jets.isValid()) {
       edm::LogPrint("MyAnalyzer") << "ERROR: Jets no validos!";
       return;
   }
   edm::LogPrint("MyAnalyzer") << "Numero de jets: " << jets->size();

   // Limpiar vectores
   j_pt.clear(); j_eta.clear(); j_phi.clear(); j_mass.clear();
   j_SDmass.clear(); j_tau1.clear(); j_tau2.clear();
   p_pt.clear(); p_eta.clear(); p_phi.clear(); p_e.clear();
   p_dxy.clear(); p_dz.clear(); p_pdgid.clear();
   j_tau21.clear();

   long long eventId = iEvent.id().event();

   int jetCounter = 0;
   for (const auto& jet : *jets) {
       edm::LogPrint("MyAnalyzer") << "  Jet " << jetCounter++
                                   << " pt=" << jet.pt()
                                   << " eta=" << jet.eta();
       if (jet.pt() < 450.0) {
           edm::LogPrint("MyAnalyzer") << "    -> RECHAZADO pt < 450";
           continue;
       }
       if (std::abs(jet.eta()) > 2.4) {
           edm::LogPrint("MyAnalyzer") << "    -> RECHAZADO |eta| > 2.4";
           continue;
       }

       edm::LogPrint("MyAnalyzer") << "    -> JET ACEPTADO, userFloats:";
       for (const std::string& name : jet.userFloatNames()) {
           edm::LogPrint("MyAnalyzer") << "      -> " << name;
       }

       float pt    = jet.pt();
       float eta   = jet.eta();
       float phi   = jet.phi();
       float mass  = jet.mass();
       float sd_mass = -1.0, t1 = -1.0, t2 = -1.0;





	//Keys correctas para tu sample
	if (jet.hasUserFloat("ak8PFJetsPuppiSoftDropMass"))
    	sd_mass = jet.userFloat("ak8PFJetsPuppiSoftDropMass");

	if (jet.hasUserFloat("NjettinessAK8Puppi:tau1"))
    	t1 = jet.userFloat("NjettinessAK8Puppi:tau1");

	if (jet.hasUserFloat("NjettinessAK8Puppi:tau2"))
    	t2 = jet.userFloat("NjettinessAK8Puppi:tau2");

        edm::LogPrint("MyAnalyzer") << "    -> sd_mass=" << sd_mass
                             << " tau1=" << t1
                             << " tau2=" << t2;
   



      // Calcular ratios (evitar division por cero)
      float tau21 = (t1 > 0) ? t2 / t1 : -1.0;

      edm::LogPrint("MyAnalyzer") << "    -> tau21=" << tau21;


       // Constituyentes
       std::vector<float> vec_pt, vec_eta, vec_phi, vec_e, vec_dxy, vec_dz;
       std::vector<int> vec_pdgid;
       vec_pt.reserve(50); vec_eta.reserve(50); vec_phi.reserve(50);
       vec_e.reserve(50);  vec_dxy.reserve(50); vec_dz.reserve(50);
       vec_pdgid.reserve(50);

       unsigned int n_daughters = jet.numberOfDaughters();
       for (unsigned int i = 0; i < n_daughters; ++i) {
           if (vec_pt.size() >= 50) break;
           const pat::PackedCandidate* c =
               dynamic_cast<const pat::PackedCandidate*>(jet.daughter(i));
           if (c) {
               vec_pt.push_back(c->pt());
               vec_eta.push_back(c->eta());
               vec_phi.push_back(c->phi());
               vec_e.push_back(c->energy());
               vec_pdgid.push_back(c->pdgId());
               vec_dxy.push_back(c->dxy());
               vec_dz.push_back(c->dz());
           }
       }

       // Llenar TTree
       j_pt.push_back(pt);       j_eta.push_back(eta);
       j_phi.push_back(phi);     j_mass.push_back(mass);
       j_SDmass.push_back(sd_mass);
       j_tau1.push_back(t1);     j_tau2.push_back(t2);
       j_tau21.push_back(tau21);

    

       p_pt.push_back(vec_pt);   p_eta.push_back(vec_eta);
       p_phi.push_back(vec_phi); p_e.push_back(vec_e);
       p_pdgid.push_back(vec_pdgid);
       p_dxy.push_back(vec_dxy); p_dz.push_back(vec_dz);

       // Escribir CSV
       csvFile_ << std::fixed << std::setprecision(6);
       csvFile_ << eventId << "," << pt << "," << eta << "," << phi << ","
                << mass << "," << sd_mass << "," << t1 << "," << t2 << "," << tau21 << ",";

       auto writeVecF = [&](const std::vector<float>& v, bool last=false) {
           csvFile_ << "\"";
           for (size_t k = 0; k < v.size(); ++k)
               csvFile_ << v[k] << (k == v.size()-1 ? "" : ";");
           csvFile_ << (last ? "\"\n" : "\",");
       };
       auto writeVecI = [&](const std::vector<int>& v) {
           csvFile_ << "\"";
           for (size_t k = 0; k < v.size(); ++k)
               csvFile_ << v[k] << (k == v.size()-1 ? "" : ";");
           csvFile_ << "\",";
       };

       writeVecF(vec_pt);
       writeVecF(vec_eta);
       writeVecF(vec_phi);
       writeVecF(vec_e);
       writeVecI(vec_pdgid);
       writeVecF(vec_dxy);
       writeVecF(vec_dz, true);  // ultimo campo, cierra con \n

   } //fin for jets

   if (!j_pt.empty()) tree_->Fill();

} //fin analyze()

DEFINE_FWK_MODULE(MyAnalyzer);

