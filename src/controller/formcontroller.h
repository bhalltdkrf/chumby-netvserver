/**
  @file
  @author Stefan Frings
  @version $Id: formcontroller.h 938 2010-12-05 14:29:58Z stefan $
*/

#ifndef FORMCONTROLLER_H
#define FORMCONTROLLER_H

#include "httprequesthandler.h"

/**
  This controller displays a HTML form and dumps the submitted input.
*/


class FormController : public HttpRequestHandler {
    Q_OBJECT
    Q_DISABLE_COPY(FormController);
public:

    /** Constructor */
    FormController();

    /** Generates the response */
    void service(HttpRequest& request, HttpResponse& response);
};

#endif // FORMCONTROLLER_H
