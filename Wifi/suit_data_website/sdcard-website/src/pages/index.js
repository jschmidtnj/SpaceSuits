import React from 'react';
import PropTypes from 'prop-types';
import Layout from 'components/layout';
import Box from 'components/box';
import Title from 'components/title';
//import Gallery from 'components/gallery';
//import IOExample from 'components/io-example';
//import Modal from 'containers/modal';
import { graphql } from 'gatsby';
import Data from 'components/data';

const Index = ({ data }) => (
  <Layout>
    <Box>
      <Title as="h2" size="large">
        {data.homeJson.content.childMarkdownRemark.rawMarkdownBody}
      </Title>
      {/*<Modal>
        <video
          src="https://vimeo.com/248591160"
          playsInline
          loop
          autoPlay
          muted
        />
      </Modal>*/}
    </Box>
    {/*<Gallery items={data.homeJson.gallery} />
    <div style={{ height: '50vh' }} />*/}
    {/*<IOExample />*/}
    <Data
      interval={data.site.siteMetadata.dataInterval}
      getSuitDataUrl={data.site.siteMetadata.getSuitDataUrl}
      getTaskDataUrl={data.site.siteMetadata.getTaskDataUrl}
    />
  </Layout>
);

Index.propTypes = {
  data: PropTypes.object.isRequired,
};

export default Index;

export const query = graphql`
  query HomepageQuery {
    homeJson {
      title
      content {
        childMarkdownRemark {
          html
          rawMarkdownBody
        }
      }
      gallery {
        title
        copy
        image {
          childImageSharp {
            fluid(maxHeight: 500, quality: 90) {
              ...GatsbyImageSharpFluid_withWebp
            }
          }
        }
      }
    }
    site {
      siteMetadata {
        dataInterval
        getSuitDataUrl
        getTaskDataUrl
      }
    }
  }
`;
