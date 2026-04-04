import FWCore.ParameterSet.Config as cms
from FWCore.ParameterSet.VarParsing import VarParsing

# 1. Configurar parsing de argumentos
# Al usar 'analysis', inputFiles y outputFile YA EXISTEN automaticamente.
options = VarParsing('analysis')
# NO registramos inputFiles ni outputFile manualmente para evitar el error.
# Solo parseamos lo que venga de la linea de comandos.
options.setDefault('outputFile', 'output.root')
options.setDefault('inputFiles', 'file:/code/63167/ED843FD2-B7DB-024D-A83F-21BC83447F8E.root')
options.parseArguments()

process = cms.Process("MYANALYSIS")

# 1. Cargar servicios estandar
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load("Configuration.StandardSequences.GeometryRecoDB_cff")
process.load("Configuration.StandardSequences.MagneticField_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")

# 2. Configurar el GlobalTag
# Usamos 'auto:run2_mc' para que CMSSW elija automaticamente el tag correcto para Run2 MC.
# Si usas DATOS reales, cambia esto a 'auto:run2_data' o el tag especifico de tu campana.
from Configuration.AlCa.GlobalTag import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, 'auto:run2_mc', '')

# Logger para evitar spam en consola
process.MessageLogger.cerr.FwkReport.reportEvery = 1000
process.MessageLogger.cerr.threshold = 'INFO'


# Usamos -1 para procesar todos los eventos que le pasemos en el chunk
#process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(1000) )

# 2. Configurar el Source usando las opciones parseadas
process.source = cms.Source("PoolSource",
    # Usamos options.inputFiles directo (que ya es una lista de strings)
    fileNames = cms.untracked.vstring(options.inputFiles),
)

# 3. Configurar el Output usando options.outputFile
process.TFileService = cms.Service("TFileService",
    fileName = cms.string(options.outputFile)
)

# --- NUEVO: Generar nombre para el CSV ---
# Si el output es "datos.root", el csv sera "datos.csv"
csv_filename = options.outputFile.replace(".root", ".csv")
# Si por alguna razon no termina en .root, aseguramos que tenga extension
if not csv_filename.endswith(".csv"):
    csv_filename += ".csv"

# 4. Configurar tu Dumper (Analyzer)
process.dumper = cms.EDAnalyzer('MyAnalyzer',
    jets = cms.InputTag("slimmedJetsAK8"),
    # Trigger results estandar
    triggerResults = cms.InputTag("TriggerResults", "", "HLT"),
    # NUEVO PARAMETRO: Pasamos el nombre del CSV a C++
    outputCSV = cms.string(csv_filename)
)

process.p = cms.Path(process.dumper)
