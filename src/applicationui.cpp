/*
 * Copyright (c) 2011-2014 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>

#include <bb/system/InvokeRequest>
#include <bb/system/InvokeTargetReply>
#include <bb/system/InvokeReplyError>
#include <bb/system/InvokeManager>
#include <QTimer>

using namespace bb::cascades;

ApplicationUI::ApplicationUI() :
        QObject()
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    bool res = QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));
    // This is only available in Debug builds
    Q_ASSERT(res);
    // Since the variable is not used in the app, this is added to avoid a
    // compiler warning
    Q_UNUSED(res);

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("_app", this);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    Application::instance()->setScene(root);

    connect(Application::instance(), SIGNAL(thumbnail()), this, SLOT(onThumbnail()));
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale().name();
    QString file_name = QString("InvokeItself_%1").arg(locale_string);
    if (m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}

void ApplicationUI::onThumbnail()
{
    qDebug() << "Thumbnailed";
    QTimer::singleShot(2000, this, SLOT(onTimer()));
}

void ApplicationUI::onTimer()
{
    qDebug() << "Invoke";

    //invoke Clock app
     bb::system::InvokeRequest request;
     request.setTarget("bb.clock.launcher");
     request.setAction("bb.action.OPEN");

     bb::system::InvokeManager *invokeManager = new  bb::system::InvokeManager();

     invokeManager->invoke(request);

     bb::system::InvokeTargetReply *reply = invokeManager->invoke(request);

     if (reply) {
       switch (reply->error()) {
           case bb::system::InvokeReplyError::NoTarget:
           {
               qDebug()<<"Target not found";
               break;
           }
           case bb::system::InvokeReplyError::BadRequest:
           {
               qDebug()<<"Bad request";
               break;
           }
           case bb::system::InvokeReplyError::Internal:
           {
               qDebug()<<"Internal error";
               break;
           }
           default:
           {
               qDebug()<<"No errors found, carried out successfully";

               // This is the message being sent to console when invoking while minimized
               // So the invocation is sent, but nothing happens...

               break;
           }
       }
     }
     else
     {
      qWarning() << "Invoke Failed! InvokeReply is empty.";
     }
}


void ApplicationUI::click()
{
    //invoke Clock app
     bb::system::InvokeRequest request;
     request.setTarget("bb.clock.launcher");
     request.setAction("bb.action.OPEN");

     bb::system::InvokeManager *invokeManager = new  bb::system::InvokeManager();

     invokeManager->invoke(request);
}
