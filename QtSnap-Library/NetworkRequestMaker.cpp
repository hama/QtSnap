#include "NetworkRequestMaker.h"

NetworkRequestMaker::NetworkRequestMaker(QString url, QUrlQuery params){
    //Initialize variables
    this->manager = new QNetworkAccessManager(this);
    this->request = QNetworkRequest(QUrl(url));

    //Add parameters to a byte array
    this->parametersByteArray.append(params.toString());

    //Connect signals
    connect(this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
}

NetworkRequestMaker::NetworkRequestMaker(QString url, QList<QHttpPart> params){
    //Initialize variables
    this->manager = new QNetworkAccessManager();
    this->httpMultiPart = new QHttpMultiPart();
    this->request = QNetworkRequest(QUrl(url));

    //Add parameters to QHttpMultiPart
    for(int i = 0; i < params.size(); i++){
        this->httpMultiPart->append(params.at(i));
    }

    //Connect signals
    connect(this->manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
}

NetworkRequestMaker::~NetworkRequestMaker()
{
    delete manager;

    if(httpMultiPart){
        delete httpMultiPart;
    }
}

void NetworkRequestMaker::executeRequest(){
    //Setup headers
    request.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    request.setHeader(QNetworkRequest::UserAgentHeader, "Snapchat/8.1.0.8 Beta (A0001; Android 21; gzip)");

    QNetworkReply *reply;

    if(this->httpMultiPart){
        request.setHeader(QNetworkRequest::ContentTypeHeader, QHttpMultiPart::FormDataType);
        //Execute POST request
        reply = this->manager->post(this->request, this->httpMultiPart);

        //Make sure the reply gets deleted when QHttpMultiPart is deleted
        this->httpMultiPart->setParent(reply);
    }else{
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

        //Execute POST request
        reply = this->manager->post(this->request, this->parametersByteArray);

        //Ignore certificates error in case of proxy
        reply->ignoreSslErrors();

        //Make sure the reply gets deleted when QNetworkAccessManager is deleted
        this->manager->setParent(reply);
    }
}

void NetworkRequestMaker::finished(QNetworkReply *reply){
    emit onRequestDone(QVariant(reply->attribute(QNetworkRequest::HttpStatusCodeAttribute)).toInt(), reply->readAll());
}