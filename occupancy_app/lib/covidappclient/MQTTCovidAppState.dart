
import 'package:flutter/cupertino.dart';

enum MQTTCovidAppConnectionState 
{ 
connected, disconnected, connecting 
}
class MQTTCovidAppState with ChangeNotifier{
  MQTTCovidAppConnectionState myappConnectionState = MQTTCovidAppConnectionState.disconnected;
  String myreceivedText = '';
  String myhistoryText = '';

  void setReceivedText(String text) {
    myreceivedText = text;
    myhistoryText = myhistoryText + '\n' + myreceivedText;
    notifyListeners();
  }
  void setAppConnectionState(MQTTCovidAppConnectionState state) {
    myappConnectionState = state;
    notifyListeners();
  }

  String get getReceivedText => myreceivedText;
  String get getHistoryText => myhistoryText;
  MQTTCovidAppConnectionState get getAppConnectionState => myappConnectionState;

}