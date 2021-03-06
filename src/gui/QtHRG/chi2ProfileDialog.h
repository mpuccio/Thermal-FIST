#ifdef USE_MINUIT

#ifndef CHI2PROFILEDIALOG_H
#define CHI2PROFILEDIALOG_H

#include <QDialog>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>
#include <QThread>
#include <QComboBox>
#include <QTimer>
#include <map>

#include "HRGFit/ThermalModelFit.h"
#include "BaseStructures.h"

//class ThermalModelFit;
class QCPColorMap;
class QCustomPlot;
class QCPColorScale;

class chi2ProfileWorker : public QThread
{
    Q_OBJECT

    ThermalModelFit *modelFit;
    int *currentSize;
    int *stop;

		std::string ParameterName;
    std::vector< double > *params;
    std::vector<double> *Avalues;
    //std::vector<double> *muBvalues;

    void run() Q_DECL_OVERRIDE {
        for(int i=0;i<Avalues->size() && !(*stop);++i) {
            double tmpParam = Avalues->operator [](i);
						if (ParameterName == "T" || ParameterName == "muB" ||
							ParameterName == "muQ" || ParameterName == "muS" ||
							ParameterName == "muC")
							tmpParam /= 1.e3; // MeV to GeV

						modelFit->SetParameterFitFlag(ParameterName, false);
						modelFit->SetParameter(ParameterName, tmpParam, 0.1, Avalues->operator [](0), Avalues->operator [](Avalues->size() - 1));
						ThermalModelFitParameters res = modelFit->PerformFit();
						params->operator[](i) = res.chi2; // modelFit->chi2Ndf(tmpT*1.e-3, tmpmuB*1.e-3);
            (*currentSize)++;
        }
        emit calculated();
    }

public:
	chi2ProfileWorker(ThermalModelFit *mod = NULL,
					 std::string inParameterName = "T",
           std::vector<double> *Avalueso = NULL,
           std::vector<double> *paramso = NULL,
           int *currentSizeo = NULL,
           int *stopo = NULL,
           QObject * parent = 0) :
        QThread(parent) {
            modelFit = mod;
            params = paramso;
						ParameterName = inParameterName;
            Avalues = Avalueso;
            currentSize = currentSizeo;
            stop = stopo;
    }
signals:
    void calculated();
};

class chi2ProfileDialog : public QDialog
{
    Q_OBJECT

    //std::vector<double> params;
    //std::vector<double> Avalues;
    //int fPreviousSize;
    //int fCurrentSize;
    //int fTotalSize;

		std::vector< std::vector<double> > vecParams;
		std::vector< std::vector<double> > vecAvalues;
		std::vector< double > vecAleft, vecAright;
		//std::vector< int > vecPreviousSize;
		std::vector< int > vecCurrentSize;
		std::vector< int > vecTotalSize;

    bool fRunning;
    int fStop;

		ThermalModelBase *model;
		ThermalParticleSystem *TPS;
    ThermalModelFit *modelFit, *modelFitInput;
		ThermalModelFitParameters fitParams;
		ThermalModelConfig config;
		std::map <int, std::string> paramNamesMap;

		std::vector<FittedQuantity> quantities;

		QLabel *labelAmin, *labelAmax, *labelAiter;

    QCustomPlot *plot;
    //QCPColorMap *colormap;
    //QCPColorScale *colorScale;

		QComboBox *comboParameter;

    QDoubleSpinBox *spinAmin, *spinAmax;
    QSpinBox *spinAiter;

    QDoubleSpinBox *spinchi2min, *spinchi2max;

    QPushButton *buttonCalculate;
    QPushButton *buttonReplot;
		QPushButton *buttonToFile;

    QProgressBar *progBar;
    QTimer *calcTimer;

    QString GetParameters();
    QString GetResults();

		QString lastFilePath;
public:
    explicit  chi2ProfileDialog(QWidget *parent, ThermalParticleSystem *inTPS, const ThermalModelConfig & inConfig, const ThermalModelFitParameters & inParams, const std::vector<FittedQuantity> & inQuantities, ThermalModelFit *inFit = NULL);

signals:
public slots:
    void calculate();
    void replot();
    //void replotpro();
    void finalize();
    void updateProgress();
		void parameterChanged(int index);
		void limitsChanged();
		void writetoFile();
private:
	void setModel();
};

#endif // CHI2PROFILEDIALOG_H

#endif
