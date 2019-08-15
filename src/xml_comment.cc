#include <node.h>

#include <cstring>

#include "libxmljs.h"

#include "xml_comment.h"
#include "xml_document.h"
#include "xml_attribute.h"
#include "xml_xpath_context.h"

namespace libxmljs {

Nan::Persistent<v8::FunctionTemplate> XmlComment::constructor_template;

// doc, content
NAN_METHOD(XmlComment::New) {
  Nan::HandleScope scope;

  // if we were created for an existing xml node, then we don't need
  // to create a new node on the document
  if (info.Length() == 0) {
      return info.GetReturnValue().Set(info.Holder());
  }

  XmlDocument* document = Nan::ObjectWrap::Unwrap<XmlDocument>(Nan::To<v8::Object>(info[0]).ToLocalChecked());
  assert(document);

  v8::Local<v8::Value> contentOpt;
  if (info[1]->IsString()) {
      contentOpt = info[1];
  }
  Nan::Utf8String contentRaw(contentOpt);
  const char* content = (contentRaw.length()) ? *contentRaw : NULL;

  xmlNode* comm = xmlNewDocComment(document->xml_obj, (xmlChar *) content);

  XmlComment* comment = new XmlComment(comm);
  comm->_private = comment;
  comment->Wrap(info.Holder());

  // this prevents the document from going away
  Nan::Set(info.Holder(), Nan::New<v8::String>("document").ToLocalChecked(), info[0]);

  return info.GetReturnValue().Set(info.Holder());
}

v8::Local<v8::Object>
XmlComment::New(xmlNode* node)
{
    Nan::EscapableHandleScope scope;
    if (node->_private) {
        return scope.Escape(static_cast<XmlComment*>(node->_private)->handle());
    }

    XmlComment* comment = new XmlComment(node);
    v8::Local<v8::Object> obj = Nan::NewInstance(Nan::GetFunction(Nan::New(constructor_template)).ToLocalChecked()).ToLocalChecked();
    comment->Wrap(obj);
    return scope.Escape(obj);
}

XmlComment::XmlComment(xmlNode* node)
    : XmlNonAttributeNode(node)
{
}

void
XmlComment::Initialize(v8::Local<v8::Object> target)
{
    Nan::HandleScope scope;
    v8::Local<v8::FunctionTemplate> t = Nan::New<v8::FunctionTemplate>(static_cast<NAN_METHOD((*))>(New));
    t->Inherit(Nan::New(XmlNonAttributeNode::constructor_template));
    t->InstanceTemplate()->SetInternalFieldCount(1);
    constructor_template.Reset( t);

    Nan::Set(target, Nan::New<v8::String>("Comment").ToLocalChecked(),
            Nan::GetFunction(t).ToLocalChecked());
}

}  // namespace libxmljs
