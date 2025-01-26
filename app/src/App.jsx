import { useState } from 'react';
import Search from './Search';
import Colour, * as matisse from "matisse";
import SearchResults from './SearchResults';
import Globe from './Globe';

function App() {
  const [queryResult, setQueryResult] = useState(null);
  const [searchResults, setSearchResults] = useState([]);
  const [loadingSearchResults, setLoadingSearchResults] = useState(false);
  const [displayColour1, setDisplayColour1] = useState(null);
  const [displayColour2, setDisplayColour2] = useState(null);

  const getSuggestions = async (prefix) => {
    const response = await fetch(`/suggest?prefix=${prefix}&n=8`);
    const data = await response.json();
    return data;
  }

  const getSearchResults = async (word) => {
    const response = await fetch(`/search?word=${word}&n=10`);
    const data = await response.json();
    const filteredData = data
      .filter(result => result.word !== word)
      .sort((a, b) => b.similarity - a.similarity);
    return filteredData;
  }

  const generateDisplayColours = () => {
    let [, colour1] = matisse.fixContrast(new Colour("#000"), matisse.randoms(1)[0], false, true);
    let [, colour2] = matisse.fixContrast(new Colour("#000"), matisse.complementary(colour1)[1], false, true);
    return [colour1.toHEX(), colour2.toHEX()];
  }

  const submitSearch = async (searchThing) => {
    setLoadingSearchResults(true);
    setQueryResult(searchThing);

    let [colour1, colour2] = generateDisplayColours();
    setDisplayColour1(colour1);
    setDisplayColour2(colour2);

    const newSearchResults = await getSearchResults(searchThing.word);
    setSearchResults(newSearchResults);
    setLoadingSearchResults(false);
  }

  const renderHeader = () => {
    return (
      <div className='header'>
        <h1>Wordtopia</h1>
        <div className='siteDescription'>Wordtopia is an AI dictionary + thesaurus powered by OpenAI language models. We generated definitions and encoded vector representations for over 475K words to study the AI's internal understanding of words in their semantic contexts. Use our custom vector database to find words with similar embeddings (vibes).</div>
      </div>
    );
  }

  const renderContent = () => {
    if (!queryResult) {
      return renderSearch();
    } else {
      return renderResults();
    }
  }

  const renderSearch = () => {
    return (
      <Search getSuggestions={getSuggestions} submitSearch={submitSearch} />
    );
  }

  const renderResults = () => {
    return (
      <div className='resultsContainer'>
        <div>
          {renderQueryResult()}
          {renderExitResultsButton()}
        </div>
        {!loadingSearchResults && searchResults.length > 0 && renderSearchResults()}
      </div>
    );
  }

  const renderQueryResult = () => {
    return (
      <div className="queryResultContainer" style={{ backgroundColor: displayColour1 }}>
        <span id="queryResultTitle">{queryResult.word}</span>
        <span id="queryResultDefinition">{queryResult.definition}</span>
      </div>
    );
  };

  const renderExitResultsButton = () => {
    const handleExit = (e) => {
      setQueryResult(null);
      setSearchResults([]);
      setLoadingSearchResults(false);
      setDisplayColour1(null);
      setDisplayColour2(null);
    }

    return (
      <div className='exitButtonContainer'>
        <button onClick={handleExit}>Search another word</button>
      </div>
    );
  }

  const renderSearchResults = () => {
    return (
      <SearchResults
        searchResults={searchResults}
        queryResultColour={displayColour1}
        alternateColour={displayColour2}
      />
    );
  }

  const renderGitHubButton = () => {
    return (
      <a href="https://github.com/tsaruggan/wordtopia" target="_blank" rel="noopener noreferrer" className="githubButton">
        GITHUB
      </a>
    );
  }

  const renderGlobe = () => {
    return (
      <Globe />
    );
  }

  return (
    <>
      <div className='page'>
        <div className='main'>
          {renderGitHubButton()}
          {renderGlobe()}
          {renderHeader()}
          {renderContent()}
        </div>
      </div>
    </>
  );
}

export default App;
