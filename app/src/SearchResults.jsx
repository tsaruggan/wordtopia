import React from 'react';
import Colour, * as matisse from "matisse";

export default function SearchResults({ searchResults, queryResultColour, alternateColour }) {
    const renderSearchResult = (result, minSim, maxSim) => {
        const { word, definition, similarity } = result;
        const normSimilarity = (similarity - minSim) / (maxSim - minSim || 1) * 0.9;
        const blendedColour = matisse.mix(new Colour(queryResultColour), new Colour(alternateColour), 1 - normSimilarity).toHEX();
        return (
            <div key={word} className="searchResultContainer" style={{ backgroundColor: blendedColour }}>
                <span id="searchResultTitle">{word}</span>
                <span id="searchResultDefinition">{definition}</span>
            </div>
        );
    };

    const minSim = Math.min(...searchResults.map(result => result.similarity));
    const maxSim = Math.max(...searchResults.map(result => result.similarity));
    return (
        <div className="searchResultsContainer">
            <h3>Has similar vibes to...</h3>
            {searchResults.map(result =>
                renderSearchResult(result, minSim, maxSim)
            )}
        </div>
    );
}