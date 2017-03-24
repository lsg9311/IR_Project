/**
 * @file porter2_stemmer.cpp
 * @author Sean Massung
 * @date September 2012
 *
 * Implementation of
 * http://snowball.tartarus.org/algorithms/english/stemmer.html
 *
 * Copyright (C) 2012 Sean Massung
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 *of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to
 *do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 *all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <algorithm>
#include <utility>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include "final_stemmer.h"

using namespace Porter2Stemmer::internal;

void Porter2Stemmer::stem(std::string& word)
{
    // special case short words or sentence tags
    if (word.size() <= 2 || word == "<s>" || word == "</s>")
        return;

    // max word length is 35 for English
    if (word.size() > 35)
        word = word.substr(0, 35);

    if (word[0] == '\'')
        word = word.substr(1, word.size() - 1);

    if (special(word))
        return;

    changeY(word);
    size_t startR1 = getStartR1(word);
    size_t startR2 = getStartR2(word, startR1);

	if (step0(word)) {
		return;
	}

    if (step1A(word))
    {
        std::replace(word.begin(), word.end(), 'Y', 'y');
        return;
    }

    step1B(word, startR1);
    step1C(word);
    step2(word, startR1);
    step3(word, startR1, startR2);
    step4(word, startR2);
    step5(word, startR1, startR2);

    std::replace(word.begin(), word.end(), 'Y', 'y');
    return;
}

void Porter2Stemmer::trim(std::string& word)
{
    if (word == "<s>" || word == "</s>")
        return;

    std::transform(word.begin(), word.end(), word.begin(), ::tolower);
    std::remove_if(word.begin(), word.end(), [](char ch)
                   {
                       return !((ch >= 'a' && ch <= 'z') || ch == '\'');
                   });
}

size_t Porter2Stemmer::internal::getStartR1(const std::string& word)
{
    // special cases
    if (word.size() >= 5 && word[0] == 'g' && word[1] == 'e' && word[2] == 'n'
        && word[3] == 'e' && word[4] == 'r')
        return 5;
    if (word.size() >= 6 && word[0] == 'c' && word[1] == 'o' && word[2] == 'm'
        && word[3] == 'm' && word[4] == 'u' && word[5] == 'n')
        return 6;
    if (word.size() >= 5 && word[0] == 'a' && word[1] == 'r' && word[2] == 's'
        && word[3] == 'e' && word[4] == 'n')
        return 5;
	//새로 추가한 사항
	//organ
	if (word.size() >= 5 && word[0] == 'o' && word[1] == 'r' && word[2] == 'g'
		&& word[3] == 'a' && word[4] == 'n')
		return 5;
	//numer
	if (word.size() >= 5 && word[0] == 'n' && word[1] == 'u' && word[2] == 'm'
		&& word[3] == 'e' && word[4] == 'r')
		return 5;
	//univer
	if (word.size() >= 6 && word[0] == 'u' && word[1] == 'n' && word[2] == 'i'
		&& word[3] == 'v' && word[4] == 'e' && word[5] == 'r')
		return 6;

    // general case
    return firstNonVowelAfterVowel(word, 1);
}

size_t Porter2Stemmer::internal::getStartR2(const std::string& word,
                                            size_t startR1)
{
    if (startR1 == word.size())
        return startR1;

    return firstNonVowelAfterVowel(word, startR1 + 1);
}

size_t
    Porter2Stemmer::internal::firstNonVowelAfterVowel(const std::string& word,
                                                      size_t start)
{
    for (size_t i = start; i != 0 && i < word.size(); ++i)
    {
        if (!isVowelY(word[i]) && isVowelY(word[i - 1]))
            return i + 1;
    }

    return word.size();
}

void Porter2Stemmer::internal::changeY(std::string& word)
{
    if (word[0] == 'y')
        word[0] = 'Y';

    for (size_t i = 1; i < word.size(); ++i)
    {
        if (word[i] == 'y' && isVowel(word[i - 1]))
            word[i++] = 'Y'; // skip next iteration
    }
}

/**
  Step 0
  's를 지운이후 고유명사인지 확인
  지우기
*/
bool Porter2Stemmer::internal::step0(std::string& word)
{
	replaceIfExists(word, "'s", "", 0);
	if (isProperNoun(word)) return true;
    // short circuit the longest suffix
    replaceIfExists(word, "'s'", "", 0) || replaceIfExists(word, "'", "", 0);
	return false;
}

/**
  Step 1a:

  sses
    replace by ss

  ied   ies
    replace by i if preceded by more than one letter, otherwise by ie
    (so ties -> tie, cries -> cri)

  us   ss
    do nothing

  s
    delete if the preceding word part contains a vowel not immediately before
  the
    s (so gas and this retain the s, gaps and kiwis lose it)
*/
bool Porter2Stemmer::internal::step1A(std::string& word)
{
    if (!replaceIfExists(word, "sses", "ss", 0))
    {
        if (endsWith(word, "ied") || endsWith(word, "ies"))
        {
            // if preceded by only one letter
            if (word.size() <= 4)
                word.pop_back();
            else
            {
                word.pop_back();
                word.pop_back();
            }
        }
        else if (endsWith(word, "s") && !endsWith(word, "us")
                 && !endsWith(word, "ss"))
        {
            if (word.size() > 2 && containsVowel(word, 0, word.size() - 2))
                word.pop_back();
        }
    }

    // special case after step 1a + heading
    return word == "inning" || word == "outing" || word == "canning"
           || word == "herring" || word == "earring" || word == "proceed"
           || word == "exceed" || word == "succeed" || word=="heading";
}

/**
  Step 1b:

  eed   eedly
      replace by ee if in R1

  ed   edly   ing   ingly
      delete if the preceding word part contains a vowel, and after the
  deletion:
      if the word ends at, bl or iz add e (so luxuriat -> luxuriate), or
      if the word ends with a double remove the last letter (so hopp -> hop), or
      if the word is short, add e (so hop -> hope)
*/
void Porter2Stemmer::internal::step1B(std::string& word, size_t startR1)
{
    bool exists = endsWith(word, "eedly") || endsWith(word, "eed");

    if (exists) // look only in startR1 now
        replaceIfExists(word, "eedly", "ee", startR1)
            || replaceIfExists(word, "eed", "ee", startR1);
    else
    {
        size_t size = word.size();
        bool deleted = (containsVowel(word, 0, size - 2)
                        && replaceIfExists(word, "ed", "", 0))
                       || (containsVowel(word, 0, size - 4)
                           && replaceIfExists(word, "edly", "", 0))
                       || (containsVowel(word, 0, size - 3)
                           && replaceIfExists(word, "ing", "", 0))
                       || (containsVowel(word, 0, size - 5)
                           && replaceIfExists(word, "ingly", "", 0));

        if (deleted && (endsWith(word, "at") || endsWith(word, "bl")
                        || endsWith(word, "iz")))
            word.push_back('e');
        else if (deleted && endsInDouble(word))
            word.pop_back();
        else if (deleted && startR1 == word.size() && isShort(word))
            word.push_back('e');
    }
}

/**
  Step 1c:

  Replace suffix y or Y by i if preceded by a non-vowel which is not the first
  letter of the word (so cry -> cri, by -> by, say -> say)
*/
void Porter2Stemmer::internal::step1C(std::string& word)
{
    size_t size = word.size();
    if (size > 2 && (word[size - 1] == 'y' || word[size - 1] == 'Y'))
        if (!isVowel(word[size - 2]))
            word[size - 1] = 'i';
}

/**
  Step 2:

  If found and in R1, perform the action indicated.

  tional:               replace by tion
  enci:                 replace by ence
  anci:                 replace by ance
  abli:                 replace by able
  entli:                replace by ent
  izer, ization:        replace by ize
  ational, ation, ator: replace by ate
  alism, aliti, alli:   replace by al
  fulness:              replace by ful
  ousli, ousness:       replace by ous
  iveness, iviti:       replace by ive
  biliti, bli:          replace by ble
  fulli:                replace by ful
  lessli:               replace by less
  ogi:                  replace by og if preceded by l
  li:                   delete if preceded by a valid li-ending
*/
void Porter2Stemmer::internal::step2(std::string& word, size_t startR1)
{
    static const std::vector<std::pair<std::string, std::string>> subs
        = {{"ational", "ate"},
           {"tional", "tion"},
           {"enci", "ence"},
           {"anci", "ance"},
           {"abli", "able"},
           {"entli", "ent"},
           {"izer", "ize"},
           {"ization", "ize"},
           {"ation", "ate"},
           {"ator", "ate"},
           {"alism", "al"},
           {"aliti", "al"},
           {"alli", "al"},
           {"fulness", "ful"},
           {"ousli", "ous"},
           {"ousness", "ous"},
           {"iveness", "ive"},
           {"iviti", "ive"},
           {"biliti", "ble"},
           {"bli", "ble"},
           {"fulli", "ful"},
           {"lessli", "less"}
	};

    for (auto& sub : subs)
        if (replaceIfExists(word, sub.first, sub.second, startR1))
            return;

    if (!replaceIfExists(word, "logi", "log", startR1 - 1))
    {
        // make sure we choose the longest suffix
        if (endsWith(word, "li") && !endsWith(word, "abli")
            && !endsWith(word, "entli") && !endsWith(word, "aliti")
            && !endsWith(word, "alli") && !endsWith(word, "ousli")
            && !endsWith(word, "bli") && !endsWith(word, "fulli")
            && !endsWith(word, "lessli"))
            if (word.size() > 3 && word.size() - 2 >= startR1
                && isValidLIEnding(word[word.size() - 3]))
            {
                word.pop_back();
                word.pop_back();
            }
    }
}

/**
  Step 3:

  If found and in R1, perform the action indicated.

  ational:            replace by ate
  tional:             replace by tion
  alize:              replace by al
  icate, iciti, ical: replace by ic
  ful, ness:          delete
  ative:              delete if in R2
*/
void Porter2Stemmer::internal::step3(std::string& word, size_t startR1,
                                     size_t startR2)
{
    static const std::vector<std::pair<std::string, std::string>> subs
        = {{"ational", "ate"},
           {"tional", "tion"},
           {"alize", "al"},
           {"icate", "ic"},
           {"iciti", "ic"},
           {"ical", "ic"},
           {"ful", ""},
           {"ness", ""}};

    for (auto& sub : subs)
        if (replaceIfExists(word, sub.first, sub.second, startR1))
            return;

    replaceIfExists(word, "ative", "", startR2);
}

/**
  Step 4:

  If found and in R2, perform the action indicated.

  al ance ence er ic able ible ant ement ment ent ism ate
    iti ous ive ize
                              delete
  ion
                              delete if preceded by s or t
*/
void Porter2Stemmer::internal::step4(std::string& word, size_t startR2)
{
    static const std::vector<std::pair<std::string, std::string>> subs
        = {{"al", ""},
           {"ance", ""},
           {"ence", ""},
           {"er", ""},
           {"ic", ""},
           {"able", ""},
           {"ible", ""},
           {"ant", ""},
           {"ement", ""},
           {"ment", ""},
           {"ism", ""},
           {"ate", ""},
           {"iti", ""},
           {"ous", ""},
           {"ive", ""},
           {"ize", ""}
	};

    for (auto& sub : subs)
        if (replaceIfExists(word, sub.first, sub.second, startR2))
            return;

    // make sure we only choose the longest suffix
    if (!endsWith(word, "ement") && !endsWith(word, "ment"))
        if (replaceIfExists(word, "ent", "", startR2))
            return;

    // short circuit
    replaceIfExists(word, "sion", "s", startR2 - 1)
        || replaceIfExists(word, "tion", "t", startR2 - 1);
}

/**
  Step 5:

  e     delete if in R2, or in R1 and not preceded by a short syllable
  l     delete if in R2 and preceded by l
*/
void Porter2Stemmer::internal::step5(std::string& word, size_t startR1,
                                     size_t startR2)
{
    size_t size = word.size();
    if (word[size - 1] == 'e')
    {
        if (size - 1 >= startR2)
            word.pop_back();
        else if (size - 1 >= startR1 && !isShort(word.substr(0, size - 1)))
            word.pop_back();
    }
    else if (word[word.size() - 1] == 'l')
    {
        if (word.size() - 1 >= startR2 && word[word.size() - 2] == 'l')
            word.pop_back();
    }
}

/**
 * Determines whether a word ends in a short syllable.
 * Define a short syllable in a word as either
 *
 * (a) a vowel followed by a non-vowel other than w, x or Y and preceded by a
 *non-vowel
 * (b) a vowel at the beginning of the word followed by a non-vowel.
 */
bool Porter2Stemmer::internal::isShort(const std::string& word)
{
    size_t size = word.size();

    if (size >= 3)
    {
        if (!isVowelY(word[size - 3]) && isVowelY(word[size - 2])
            && !isVowelY(word[size - 1]) && word[size - 1] != 'w'
            && word[size - 1] != 'x' && word[size - 1] != 'Y')
            return true;
    }
    return size == 2 && isVowelY(word[0]) && !isVowelY(word[1]);
}


bool Porter2Stemmer::internal::isProperNoun(std::string& word) {
	static const std::vector<std::string> properNouns = {
		"george","fbi","yvette","austin", "micheal", "john",
		"alabama","alaska","arizona","arkansas","california","colorado",
		"connecticut","delaware","florida","georgia","hawaii","idaho",
		"illinois","indiana","lowa","kansas","kentucky","lousiana","maine",
		"maryland","massachusetts","michigan","minnesota","missouri","montana",
		"nebraska","nevada","ohio","oklahoma","oregon","pennsylvania","tennessee", "texas",
		"utah","vermont","virginia","washington","wisconsin"
	};
	// find
	for (int i = 0; i < properNouns.size(); i++) {
		if (properNouns.at(i).compare(word)==0){
			return true;
		}
	}
	return false;

}

bool Porter2Stemmer::internal::special(std::string& word)
{
    static const std::unordered_map<std::string, std::string> exceptions
        = {{"skis", "ski"},
           {"skies", "sky"},
		//ie에 ing가 붙은 경우
           {"dying", "die"},
           {"lying", "lie"},
           {"tying", "tie"},
		//ly자체가 명사
           {"idly", "idl"},
           {"gently", "gentl"},
           {"ugly", "ugli"},
           {"early", "earli"},
           {"only", "onli"},
           {"singly", "singl"}
		,
		//krovetz에서 가져온 예외 case
		   { "suing", "sue" },
		   { "cued", "cue" },
		   { "hued", "hue" },
		   { "sued", "sue" },{ "toed", "toe" },
		   { "cylindrical", "cylinder" },
		   { "nazism", "nazi" },{ "generosity", "generous" },
		   { "voracity", "voracious" },{ "fled", "flee" },
		   { "miscarriage", "miscarry" },
		   { "afghan", "afghanistan" },{ "african", "africa" },{ "albanian", "albania" },
		   { "algerian", "algeria" },{ "american", "america" },{ "andorran", "andorra" },
		   { "angolan", "angola" },{ "arabian", "arabia" },{ "argentine", "argentina" },
		   { "armenian", "armenia" },{ "asian", "asia" },{ "australian", "australia" },
		   { "austrian", "austria" },{ "azerbaijani", "azerbaijan" },
		   { "azeri", "azerbaijan" },{ "bangladeshi", "bangladesh" },
		   { "belgian", "belgium" },{ "bermudan", "bermuda" },
		   { "bolivian", "bolivia" },{ "bosnian", "bosnia" },{ "botswanan", "botswana" },
		   { "brazilian", "brazil" },{ "british", "britain" },{ "bulgarian", "bulgaria" },
		   { "burmese", "burma" },{ "californian", "california" },{ "cambodian", "cambodia" },
		   { "canadian", "canada" },{ "chadian", "chad" },{ "chilean", "chile" },
		   { "chinese", "china" },{ "colombian", "colombia" },{ "croat", "croatia" },
		   { "croatian", "croatia" },{ "cuban", "cuba" },{ "cypriot", "cyprus" },
		   { "czechoslovakian", "czechoslovakia" },{ "danish", "denmark" },{ "egyptian", "egypt" },
		   { "equadorian", "equador" },{ "eritrean", "eritrea" },{ "estonian", "estonia" },
		   { "ethiopian", "ethiopia" },{ "european", "europe" },{ "fijian", "fiji" },
		   { "filipino", "philippines" },{ "finnish", "finland" },{ "french", "france" },
		   { "gambian", "gambia" },{ "georgian", "georgia" },{ "german", "germany" },
		   { "ghanian", "ghana" },{ "greek", "greece" },{ "grenadan", "grenada" },
		   { "guamian", "guam" },{ "guatemalan", "guatemala" },{ "guinean", "guinea" },
		   { "guyanan", "guyana" },{ "haitian", "haiti" },{ "hawaiian", "hawaii" },
		   { "holland", "dutch" },{ "hungarian", "hungary" },
		   { "icelandic", "iceland" },{ "indonesian", "indonesia" },
		   { "iranian", "iran" },{ "iraqi", "iraq" },{ "iraqui", "iraq" },{ "irish", "ireland" },
		   { "israeli", "israel" },{ "italian", "italy" },{ "jamaican", "jamaica" },{ "japanese", "japan" },
		   { "jordanian", "jordan" },{ "kampuchean", "cambodia" },{ "kenyan", "kenya" },
		   { "korean", "korea" },{ "kuwaiti", "kuwait" },{ "lankan", "lanka" },
		   { "laotian", "laos" },{ "latvian", "latvia" },{ "lebanese", "lebanon" },
		   { "liberian", "liberia" },{ "libyan", "libya" },{ "lithuanian", "lithuania" },
		   { "macedonian", "macedonia" },{ "madagascan", "madagascar" },{ "malaysian", "malaysia" },
		   { "maltese", "malta" },{ "mauritanian", "mauritania" },{ "mexican", "mexico" },
		   { "micronesian", "micronesia" },{ "moldovan", "moldova" },{ "monacan", "monaco" },
		   { "mongolian", "mongolia" },{ "montenegran", "montenegro" },{ "moroccan", "morocco" },
		   { "myanmar", "burma" },{ "namibian", "namibia" },{ "nepalese", "nepal" },
		   { "nicaraguan", "nicaragua" },{ "nigerian", "nigeria" },
		   { "norwegian", "norway" },{ "omani", "oman" },{ "pakistani", "pakistan" },
		   { "panamanian", "panama" },{ "papuan", "papua" },{ "paraguayan", "paraguay" },
		   { "peruvian", "peru" },{ "portuguese", "portugal" },{ "romanian", "romania" },
		   { "rumania", "romania" },{ "rumanian", "romania" },{ "russian", "russia" },
		   { "rwandan", "rwanda" },{ "samoan", "samoa" },{ "scottish", "scotland" },
		   { "serb", "serbia" },{ "serbian", "serbia" },{ "siam", "thailand" },
		   { "siamese", "thailand" },{ "slovakia", "slovak" },{ "slovakian", "slovak" },
		   { "slovenian", "slovenia" },{ "somali", "somalia" },{ "somalian", "somalia" },
		   { "spanish", "spain" },{ "swedish", "sweden" },{ "swiss", "switzerland" },
		   { "syrian", "syria" },{ "taiwanese", "taiwan" },{ "tanzanian", "tanzania" },
		   { "texan", "texas" },{ "thai", "thailand" },{ "tunisian", "tunisia" },
		   { "turkish", "turkey" },{ "ugandan", "uganda" },{ "ukrainian", "ukraine" },
		   { "uruguayan", "uruguay" },{ "uzbek", "uzbekistan" },{ "venezuelan", "venezuela" },
		   { "vietnamese", "viet" },{ "virginian", "virginia" },{ "yemeni", "yemen" },
		   { "yugoslav", "yugoslavia" },{ "yugoslavian", "yugoslavia" },{ "zambian", "zambia" },
		   { "zealander", "zealand" },{ "zimbabwean", "zimbabwe" },
		   { "appendices", "appendix" },
		   { "bater", "bate" },
		   { "bookshelves", "bookshelf" },
		   { "dwarves", "dwarf" },
		   { "scarves", "scarf" },
		   { "thieves", "thief" },
		   { "vortices", "vortex" },
		   { "wharves", "wharf" },
		   { "wolves", "wolf" },
		   { "yourselves", "yourself" },

		//changed
		   { "crises", "crisi" },
		   { "eerily", "eeri" },
		   { "housewives", "housewif" },
		   { "midwives", "midwif" },
		   { "oases", "oasis" },
		   { "parentheses", "parenthesi" },
		   { "synopses", "synopsi" },
		   { "syntheses", "synthesi" },
		   { "testes", "testicl" },
		   { "theses", "thesi" },
		   { "honduran", "hondura" },
		   {"found","find"}
	};

    // special cases
    auto ex = exceptions.find(word);
    if (ex != exceptions.end())
    {
        word = ex->second;
        return true;
    }

    // invariants
	//새로 추가한 사항, stem 끝에 e,s가 붙은 단어들
	
    return word == "sky" || word == "news" || word == "howe" || word == "atlas"
           || word == "cosmos" || word == "bias" || word == "andes" || word=="police" ||word=="paste" || word=="oasis";
}

bool Porter2Stemmer::internal::isVowelY(char ch)
{
    return ch == 'e' || ch == 'a' || ch == 'i' || ch == 'o' || ch == 'u'
           || ch == 'y';
}

bool Porter2Stemmer::internal::isVowel(char ch)
{
    return ch == 'e' || ch == 'a' || ch == 'i' || ch == 'o' || ch == 'u';
}

bool Porter2Stemmer::internal::endsWith(const std::string& word,
                                        const std::string& str)
{
    return word.size() >= str.size()
           && std::equal(word.begin() + (word.size() - str.size()), word.end(),
                         str.begin());
}

bool Porter2Stemmer::internal::endsInDouble(const std::string& word)
{
    if (word.size() >= 2)
    {
        char a = word[word.size() - 1];
        char b = word[word.size() - 2];

        if (a == b)
            return a == 'b' || a == 'd' || a == 'f' || a == 'g' || a == 'm'
                   || a == 'n' || a == 'p' || a == 'r' || a == 't';
    }

    return false;
}

bool Porter2Stemmer::internal::replaceIfExists(std::string& word,
                                               const std::string& suffix,
                                               const std::string& replacement,
                                               size_t start)
{
    size_t idx = word.size() - suffix.size();
    if (idx < start)
        return false;

    if (std::equal(word.begin() + idx, word.end(), suffix.begin()))
    {
        word = word.substr(0, word.size() - suffix.size()) + replacement;
        return true;
    }
    return false;
}

bool Porter2Stemmer::internal::isValidLIEnding(char ch)
{
    return ch == 'c' || ch == 'd' || ch == 'e' || ch == 'g' || ch == 'h'
           || ch == 'k' || ch == 'm' || ch == 'n' || ch == 'r' || ch == 't';
}

bool Porter2Stemmer::internal::containsVowel(const std::string& word,
                                             size_t start, size_t end)
{
    if (end <= word.size())
    {
        for (size_t i = start; i < end; ++i)
            if (isVowelY(word[i]))
                return true;
    }
    return false;
}
