#include <iostream> // For debugging
#include <stdexcept>
#include <string>
#include <set>
#include <sstream>
#include <cassert>

#include "nand/nand-parser.h"


const int FIRST_CHARACTER = 1;
const int LAST_CHARACTER = 4;

const int FIRST_CHECKSUM_OFFSET = 4;
const int LAST_CHECKSUM_OFFSET = 2;

const int NEIL_NUMBER_OF_FIELDS = 3;
const int NUNO_NUMBER_OF_FIELDS = 4;
const int ALIS_NUMBER_OF_FIELDS = 5;
const int DAVE_NUMBER_OF_FIELDS = 6;

const char START_SYMBOL = '~';
const char END_SYMBOL = ';';
const char DELIMETER_SYMBOL = '|';

const char DEGREE_SYMBOL = 'o';
const char MINUTE_SYMBOL = '\'';
const std::string SECONDS_SYMBOL  = "''";


struct fieldIndices
{
const int neilLatitudeIndex = 0;
const int neilLongitudeIndex = 1;
const int neilaltitudeIndex = 2;

const int alisLatitudeIndex = 0;
const int alisLatitudeDirection = 1;
const int alisLongitudeIndex = 2;
const int alisLongitudeDirection = 3;
const int alisAltitudeIndex = 4;

const int nunoLatitudeIndex = 3;
const int nunoLongitudeIndex = 2;
const int nunoaltitudeIndex = 1;

const int daveLatitudeIndex = 1;
const int daveLatitudeDirection = 2;
const int daveLongitudeIndex = 3;
const int daveLongitudeDirection = 4;
const int daveAltitudeIndex = 5;

};





struct pipeIndex
{

    int value;
    const unsigned int offsetFromEnd = 6;
    const int indexAfterFirstPipe = 6;

    int getLastPipeIndex(){
        return value - offsetFromEnd;
    }

};


struct validCharacters
{
    std::set<char> validCharaters {',' , '+' , '.' , '-' , 'o' , ':', '\''};

    bool isValidCharacter(char character)
    {
        return validCharaters.find(character) != validCharaters.end();
    }

};



namespace GPS::NAND
{
  bool isRecognisedCode(std::string code)
  {
      std::set<std::string> recognisedCode {"ALIS", "NUNO", "DAVE", "NEIL"};
      if(recognisedCode.find(code) == recognisedCode.end())
      {
          return false;
      }
      return true;
  }



  bool hasValidCharacters(const std::string& dataEntry, int start, int end, bool letters, bool digits) {

    for (int i = start; i <= end; ++i) {
      if ((letters && !std::isalpha(dataEntry[i])) || dataEntry[i] != std::toupper(dataEntry[i])) return false;
      if (digits && !std::isdigit(dataEntry[i])) return false;
    }

    return true;
  }



  bool isInvalidCharacter(const std::string& dataEntry, unsigned int start, unsigned int end) {
        validCharacters character;
      for (unsigned int i = start; i <= end; ++i) {
            if (!std::isalnum(dataEntry[i]) && !character.isValidCharacter(dataEntry[i])) {
                return true;
            }
        }
        return false;
    }

  bool isCharacternotInString(const std::string& dataEntry,int index, char character)
  {
      return dataEntry[index] != character;
  }



  bool hasFormOfNANDdataEntry(std::string dataEntry)
  {

      int index;

      index = 0;
      if(isCharacternotInString(dataEntry,index,START_SYMBOL))
      {
          return false;
      }

        if (!hasValidCharacters(dataEntry, FIRST_CHARACTER, LAST_CHARACTER, true, false)) {
             return false;
         }

        index = 5;
        if(isCharacternotInString(dataEntry,index,DELIMETER_SYMBOL))
           {
               return false;
           }

        pipeIndex pipeIndex;
        pipeIndex.value = dataEntry.size();


        if (isInvalidCharacter(dataEntry, pipeIndex.indexAfterFirstPipe, pipeIndex.getLastPipeIndex())) {
                 return false;
             }




        int FirstCheckSumCharacter = dataEntry.size() - FIRST_CHECKSUM_OFFSET;
        int LactCheckSumCharacter = dataEntry.size() - LAST_CHECKSUM_OFFSET;

        if (!hasValidCharacters(dataEntry, FirstCheckSumCharacter, LactCheckSumCharacter, false, true)) {
             return false;
            }

        index = dataEntry.size() - 1; //last character of the dataEntry
        if(isCharacternotInString(dataEntry,index,END_SYMBOL))
           {
               return false;
           }




      return true;
  }


/*

**USER PROMPT**
xor reduciton check sum for"+45.67,-23.24,231.56"using ascii for only numbers



**AI RESPONSE**

// Function to calculate XOR reduction checksum for numbers in a string
int calculateChecksum(const std::string& input) {
    int checksum = 0;
    for (char c : input) {
        if (c >= '0' && c <= '9') { // Check if character is a digit
            checksum ^= static_cast<int>(c); // XOR operation with ASCII value
        }
    }
    return checksum;
}

int main() {
    std::string input = "+45.67,-23.24,231.56";
    int checksum = calculateChecksum(input);
    std::cout << "XOR reduction checksum for \"" << input << "\" using ASCII values for numbers only: " << checksum << std::endl;
    return 0;
}


*/


  int calculateChecksum(const std::string& dataEntry) {
      int checksum = 0;


      pipeIndex pipeIndex;
      pipeIndex.value = dataEntry.size();

      for (int i = pipeIndex.indexAfterFirstPipe; i <= pipeIndex.getLastPipeIndex(); i++) {
          validCharacters character;
          if (std::isalnum(dataEntry[i]) || character.isValidCharacter(dataEntry[i])) { // Check if character is a digit or sign
              checksum ^= static_cast<int>(dataEntry[i]); // XOR operation with ASCII value
          }
      }
      return checksum;
  }


  bool verifyChecksum(std::string dataEntry)
  {

      assert(hasFormOfNANDdataEntry(dataEntry));

      const unsigned int positionOfChecksum = dataEntry.size() - FIRST_CHECKSUM_OFFSET;
      int correctChecksum = calculateChecksum(dataEntry);
      int dataEntryChecksum = std::stoi(dataEntry.substr(positionOfChecksum,3));

      return correctChecksum == dataEntryChecksum;
  }







  /*

**USER PROMPT**
"+45.67,-23.24,231.56" c++ implementation to split a string by commas


**AI RESPONSE**

int main() {
    std::string input = "+45.67,-23.24,231.56";
    std::istringstream ss(input);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }

    // Now tokens vector contains the split strings
    for (const auto& t : tokens) {
        std::cout << t << std::endl;
    }

    return 0;

*/




  std::vector<std::string> splitByArgument(std::string entry,char argument)
  {
      std::istringstream ss(entry);
      std::string field;
      std::vector<std::string> fields;

      while (std::getline(ss, field, argument)) {
          fields.push_back(field);
      }

      int lastItem = entry.size()-1;// Adjusting the index by -1 to ensure it points to the last element
      if(entry[lastItem] == argument)
      {
         fields.push_back("");
      }

      return fields;

  }


  NAND::DataEntry parseDataEntry(std::string dataField)
  {
      assert(hasFormOfNANDdataEntry(dataField));


      const int CHARACTERS_TO_REMOVE = 11; // Adjusting the size by subtracting the number of characters to remove
      int dataSize = dataField.size() - CHARACTERS_TO_REMOVE;
       pipeIndex pipeIndex;

      std::string nandType = dataField.substr(FIRST_CHARACTER,LAST_CHARACTER);
      std::string fields = dataField.substr(pipeIndex.indexAfterFirstPipe,dataSize);
      std::vector<std::string>separatedFields = splitByArgument(fields,',');
      return {nandType, separatedFields};
  }

  bool hasExpectedNumberOfFields(NAND::DataEntry field)
  {

    assert(isRecognisedCode(field.format));

    int numberOfFields = field.dataFields.size();

    if(field.format == "NEIL")
    {
        return numberOfFields == NEIL_NUMBER_OF_FIELDS ;
    }
    if(field.format == "NUNO")
    {
        return numberOfFields == NUNO_NUMBER_OF_FIELDS;
    }
    if(field.format == "ALIS")
    {
        return numberOfFields == ALIS_NUMBER_OF_FIELDS;
    }
    if(field.format == "DAVE")
    {
        return numberOfFields == DAVE_NUMBER_OF_FIELDS;
    }
    return false;
  }






  bool isValidField(NAND::DataEntry field)
  {
      fieldIndices indices;


      if(field.format == "NEIL")
      {
          double latitude = std::stod(field.dataFields[indices.neilLatitudeIndex]);
          if(latitude > 90.0)
          {
          return false;
          }
      }

      if(field.format == "NUNO")
      {
          double longitude =  std::stod(field.dataFields[indices.nunoLongitudeIndex]);
          if(longitude < -180.0 || longitude > 180.0)
          {
          return false;
          }
      }


      if(field.format == "ALIS")
      {
          std::string latitude = field.dataFields[indices.alisLatitudeIndex];

          if (latitude.find(DEGREE_SYMBOL) == std::string::npos)
          {
              return false;
          }

      }

      if(field.format == "DAVE")
      {
          std::string longitude = field.dataFields[indices.daveLongitudeIndex];

          if (longitude.find(SECONDS_SYMBOL) == std::string::npos)
          {
              return false;
          }

      }

      return true;
  }





/*
**USER PROMPT**
"78o36'45''" using this... i have been able to get the sybols 'o' and ''' from the string
using the find method but i am having issues storinG the position within an int variable
and then using that position to remove all characters before the symbol i am looking for
and also the symbol itself



**AI RESPONSE**
int main() {
    std::string str = "78o36'45''";

    // Find the position of 'o'
    size_t pos_o = str.find('o');
    if (pos_o != std::string::npos) {
        // Store the position in an int variable
        int position_o = static_cast<int>(pos_o);

        // Create a substring that starts from the position after 'o'
        std::string substring = str.substr(pos_o + 1);

        std::cout << "Substring after 'o': " << substring << std::endl;
    } else {
        std::cout << "'o' not found" << std::endl;
    }

    return 0;
}



*/

  double parseDMS(std::string dms) {
      int degrees = 0, minutes = 0;
      double seconds = 0.0;
      size_t pos = 0;

      // Parse degrees
      pos = dms.find(DEGREE_SYMBOL);
      if (pos == std::string::npos) {
          throw std::domain_error("Degrees symbol 'o' not found in DMS string: " + dms);
      }
      degrees = std::stoi(dms.substr(0, pos));
      dms = dms.substr(pos + 1); // Removes degree symbol and values before degree symbol

      // Parse minutes
      pos = dms.find(MINUTE_SYMBOL);
      if (pos == std::string::npos) {
          throw std::domain_error("Minutes symbol ''' not found in DMS string: " + dms);
      }
      minutes = std::stoi(dms.substr(0, pos));
      dms = dms.substr(pos + 1); // Removes minute symbol and values before minute symbol

      // Parse seconds
      pos = dms.find(SECONDS_SYMBOL);
      if (pos == std::string::npos) {
          throw std::domain_error("Seconds symbol '''' not found in DMS string: " + dms);
      }
      seconds = std::stod(dms.substr(0, pos));
      dms = dms.substr(pos + 2); // Removes seconds symbol and values before seconds symbol

      // Convert to decimal degrees
      double decimalDegrees = degrees + minutes / 60.0 + seconds / 3600.0; //converting minutes and seconds to hours
      return decimalDegrees;
  }














  Waypoint dataEntryToWaypoint(NAND::DataEntry dataEntry)
  {
      assert(isRecognisedCode(dataEntry.format));

      assert(hasExpectedNumberOfFields(dataEntry));

      std::string latitudeAsString, longitudeAsString, altitudeAsString, latitudeDirection, longitudeDirection, degreesAsString, minutesAsString, secondsAsString, temp;
      double latitudeAsDouble, longitudeAsDouble, altitudeAsDouble;
      Waypoint waypoint = Waypoint(0,0,0); // Dummy object because there's no default constructor available for Waypoint
      fieldIndices indices;



      if (dataEntry.format == "NEIL")
      {
          latitudeAsString = dataEntry.dataFields[indices.neilLatitudeIndex];
          longitudeAsString = dataEntry.dataFields[indices.neilLongitudeIndex];
          altitudeAsString = dataEntry.dataFields[indices.neilaltitudeIndex];
          try
          {
              latitudeAsDouble = stod(latitudeAsString);
              longitudeAsDouble = stod(longitudeAsString);
              altitudeAsDouble = stod(altitudeAsString);
              waypoint = Waypoint(latitudeAsDouble,longitudeAsDouble,altitudeAsDouble);

          }
          catch (const std::invalid_argument& e)
          {
              throw std::domain_error(std::string("Ill-formed NEIL data field: ") + e.what());
          }
      }
      else if (dataEntry.format == "ALIS")
      {
          latitudeAsString = dataEntry.dataFields[indices.alisLatitudeIndex];
          latitudeDirection = dataEntry.dataFields[indices.alisLatitudeDirection];
          longitudeAsString = dataEntry.dataFields[indices.alisLongitudeIndex];
          longitudeDirection = dataEntry.dataFields[indices.alisLongitudeDirection];
          altitudeAsString = dataEntry.dataFields[indices.alisAltitudeIndex];
          try
          {
              latitudeAsDouble = parseDMS(latitudeAsString);

              if (latitudeAsDouble < 0)
              {
                  throw std::invalid_argument("latitude values in DMS format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
              longitudeAsDouble = parseDMS(longitudeAsString);

              if (longitudeAsDouble < 0)
              {
                  throw std::invalid_argument("longitude values in DMS format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
              altitudeAsDouble = stod(altitudeAsString);
          }
          catch (const std::invalid_argument& e)
          {
              throw std::domain_error(std::string("Ill-formed ALIS data field: ") + e.what());
          }
          if (latitudeDirection.size() == 1) {
              switch (latitudeDirection[0])
              {
                  case 'N': break;
                  case 'S': latitudeAsDouble = -latitudeAsDouble; break;
                  default: throw std::domain_error("Ill-formed ALIS data field: " + latitudeDirection + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
              }
          } else {
              throw std::domain_error("Ill-formed ALIS data field: " + latitudeDirection + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
          }
          if (longitudeDirection.size() == 1) {
              switch (longitudeDirection[0])
              {
                  case 'E': break;
                  case 'W': longitudeAsDouble = -longitudeAsDouble; break;
                  default: throw std::domain_error("Ill-formed ALIS data field: " + longitudeDirection + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
              }
          } else {
              throw std::domain_error("Ill-formed ALIS data field: " + longitudeDirection + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
          }
          try
          {
                waypoint = Waypoint(latitudeAsDouble,longitudeAsDouble,altitudeAsDouble);
          }
          catch (const std::invalid_argument& e)
          {
                throw std::domain_error(std::string("Ill-formed ALIS data field: ") + e.what());
          }
      }


      else if (dataEntry.format == "NUNO")
      {
          latitudeAsString = dataEntry.dataFields[indices.nunoLatitudeIndex];
          longitudeAsString = dataEntry.dataFields[indices.nunoLongitudeIndex];
          altitudeAsString = dataEntry.dataFields[indices.nunoaltitudeIndex];
          try
          {
              latitudeAsDouble = stod(latitudeAsString);
              longitudeAsDouble = stod(longitudeAsString);
              altitudeAsDouble = stod(altitudeAsString);
          }
          catch (const std::invalid_argument& e)
          {
              throw std::domain_error(std::string("Ill-formed NUNO data field: ") + e.what());
          }
          try
          {
              waypoint = Waypoint(latitudeAsDouble,longitudeAsDouble,altitudeAsDouble);
          }
          catch (const std::invalid_argument& e)
          {
              throw std::domain_error(std::string("Ill-formed NUNO data field: ") + e.what());
          }
      }


      else if (dataEntry.format == "DAVE")
      {
          latitudeAsString = dataEntry.dataFields[indices.daveLatitudeIndex];
          latitudeDirection = dataEntry.dataFields[indices.daveLatitudeDirection];
          longitudeAsString = dataEntry.dataFields[indices.daveLongitudeIndex];
          longitudeDirection = dataEntry.dataFields[indices.daveLongitudeDirection];
          altitudeAsString = dataEntry.dataFields[indices.daveAltitudeIndex];
          try
          {
              latitudeAsDouble = parseDMS(latitudeAsString);


              if (latitudeAsDouble < 0)
              {
                  throw std::invalid_argument("latitude values in DMS format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
                longitudeAsDouble = parseDMS(longitudeAsString);

              if (longitudeAsDouble < 0)
              {
                  throw std::invalid_argument("longitude values in DMS format must be positive.  Positive/negative direction is denoted by a separate bearing indicator.");
              }
              altitudeAsDouble = stod(altitudeAsString);
          }
          catch (const std::invalid_argument& e)
          {
              throw std::domain_error(std::string("Ill-formed DAVE data field: ") + e.what());
          }
          if (latitudeDirection.size() == 1) {
              switch (latitudeDirection[0])
              {
                  case 'N': break;
                  case 'S': latitudeAsDouble = -latitudeAsDouble; break;
                  default: throw std::domain_error("Ill-formed DAVE data field: " + latitudeDirection + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
              }
          } else {
              throw std::domain_error("Ill-formed DAVE data field: " + latitudeDirection + " is an invalid latitude bearing indicator.  Only 'N' or 'S' accepted.");
          }
          if (longitudeDirection.size() == 1) {
              switch (longitudeDirection[0])
              {
                  case 'E': break;
                  case 'W': longitudeAsDouble = -longitudeAsDouble; break;
                  default: throw std::domain_error("Ill-formed DAVE data field: " + longitudeDirection + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
              }
          } else {
              throw std::domain_error("Ill-formed DAVE data field: " + longitudeDirection + " is an invalid longitude bearing indicator.  Only 'E' or 'W' accepted.");
          }
          try
          {
                waypoint = Waypoint(latitudeAsDouble,longitudeAsDouble,altitudeAsDouble);
          }
          catch (const std::invalid_argument& e)
          {
                throw std::domain_error(std::string("Ill-formed DAVE data field: ") + e.what());
          }
      }
      return waypoint;
  }








  std::vector<Waypoint> processDataLog(std::istream& dataLog)
  {


    std::vector<Waypoint> waypoints;
    NAND::DataEntry nandDataEntry;
    Waypoint waypoint = Waypoint(0,0,0);;



    std::string dataLogString;
           char character;
           while (dataLog.get(character)) {
               if(character != ' ' && character != '\t' && character != '\n') //checks for white spaces so they can be ignored
               {
                dataLogString += character;
               }
           }

    std::vector<std::string>separateDataEnteries = splitByArgument(dataLogString,';');
    separateDataEnteries.erase(std::remove(separateDataEnteries.begin(), separateDataEnteries.end(), ""), separateDataEnteries.end());

    for(auto dataEntry : separateDataEnteries){
        dataEntry+=';';
        std::string dataEntryFormat = dataEntry.substr(FIRST_CHARACTER,LAST_CHARACTER);


        if(!hasFormOfNANDdataEntry(dataEntry))
        {
           continue;
        }
        if(!isRecognisedCode(dataEntryFormat))
        {
           continue;
        }
        if(!verifyChecksum(dataEntry))
        {
           continue;
        }

       nandDataEntry = parseDataEntry(dataEntry);

       if(!hasExpectedNumberOfFields(nandDataEntry))
       {
          continue;
       }

       if(!isValidField(nandDataEntry))
       {
           continue;
       }


       waypoint = dataEntryToWaypoint(nandDataEntry);
       waypoints.push_back(waypoint);

    }

      return waypoints;
  }

}


















